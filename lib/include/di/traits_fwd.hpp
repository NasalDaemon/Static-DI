#ifndef INCLUDE_DI_TRAITS_FWD_HPP
#define INCLUDE_DI_TRAITS_FWD_HPP

#include "di/context_fwd.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/trait.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
template<class Interface_, class Types_>
struct ResolvedTrait
{
    using Interface = Interface_;
    using Types = Types_;
};

DI_MODULE_EXPORT
template<class T>
concept IsResolvedTrait = requires
{
    typename T::Interface;
    typename T::Types;
};

namespace detail {

    template<class Node_>
    struct TraitNodeInterface
    {
        using Node = Node_;
        using DefaultInterface = Node_;
    };

    template<class Node_, class Interface_>
    struct TraitNodeInterface<Node_(Interface_)>
    {
        using Node = Node_;
        using DefaultInterface = Interface_;
    };

    template<class TraitTs, class Trait>
    concept TraitsHasTrait = IsTrait<Trait> and requires (Trait trait) { { TraitTs::resolveTrait(trait) } -> IsResolvedTrait; };

    template<class Node_, template<class> class GetContext_, class DefaultResolver, class... TraitTs>
    struct Traits;

    struct TraitsDefault;

    template<class Node>
    struct TraitsOpenDefault;

    template<template<class> class TraitTemplate>
    struct TraitsTemplateDefault;


} // namespace detail

DI_MODULE_EXPORT
template<class Node, class... TraitTs>
using Traits = detail::Traits<Node, ContextParameterOf, detail::TraitsDefault, TraitTs...>;

DI_MODULE_EXPORT
template<class Node, class... Traits>
using TraitsOpen = detail::Traits<Node, ContextParameterOf, detail::TraitsOpenDefault<detail::TraitNodeInterface<Node>>, Traits...>;

DI_MODULE_EXPORT
template<class Node, template<class> class TraitTemplate, class... Traits>
using TraitsTemplate = detail::Traits<Node, ContextParameterOf, detail::TraitsTemplateDefault<TraitTemplate>, Traits...>;

} // namespace di


#endif // INCLUDE_DI_TRAITS_FWD_HPP
