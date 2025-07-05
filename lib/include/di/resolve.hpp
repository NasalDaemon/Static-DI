#ifndef INCLUDE_DI_RESOLVE_HPP
#define INCLUDE_DI_RESOLVE_HPP

#include "di/link.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

namespace di {

namespace detail {

    template<class T, class Trait_>
    struct ResolveTrait
    {
        using type = ResolveTrait;
        using Node = T;
        using Trait = Trait_;

        template<template<class, class> class Template>
        using Apply = Template<Node, Trait>;
    };

    template<class T, class Trait>
    requires HasLink<T, Trait>
    struct ResolveTrait<T, Trait>
    {
        using Target = ResolveLink<T, Trait>;
        using type = ResolveTrait<ContextToNode<typename Target::Context>, typename Target::Trait>::type;
    };

    // Delegate resolution to the parent cluster's context
    template<class T, class Trait>
    requires LinksToParent<T, Trait>
    struct ResolveTrait<T, Trait>
    {
        using Target = ResolveLink<T, Trait>;
        using type = ResolveTrait<typename Target::Context, typename Target::Trait>::type;
    };

} // namespace detail

DI_MODULE_EXPORT
template<class Node, IsTrait Trait>
requires detail::HasLink<ContextOf<Node>, Trait> and NodeDependencyAllowed<Node, Trait>
using ResolveTypes = detail::ResolveTrait<ContextOf<Node>, Trait>::type::template Apply<NodeTypes>;

DI_MODULE_EXPORT
template<IsContext Context>
using ResolveRoot = Context::Root;

DI_MODULE_EXPORT
template<IsContext Context>
using ResolveInfo = Context::Info;

} // namespace di


#endif // INCLUDE_DI_RESOLVE_HPP
