#ifndef INCLUDE_DI_CONTEXT_FWD_HPP
#define INCLUDE_DI_CONTEXT_FWD_HPP

#include "di/detail/concepts.hpp"

#include "di/empty_types.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct ContextBase;
}

DI_MODULE_EXPORT
template<class T>
concept IsContext = std::is_base_of_v<detail::ContextBase, T> and IsStateless<T> and requires {
    typename T::Root;
    typename T::Info;
    T::Depth;
};

DI_MODULE_EXPORT
template<class C1, class C2>
concept IsSameContext = std::same_as<detail::Decompress<C1>, detail::Decompress<C2>>;

DI_MODULE_EXPORT
template<class Context>
concept IsElementContext = IsContext<Context> and requires { typename Context::Info::ElementContext; };

namespace detail {
    template<template<class> class NodeTmpl, IsContext Context>
    constexpr Context getContextParameter(NodeTmpl<Context> const&)
    {
        return {};
    }

    template<class T>
    requires requires { typename T::Traits; }
    auto getContext() -> T::Traits::template GetContext<>;

    template<class T>
    auto getContext() -> decltype(getContextParameter(std::declval<T const&>()));
}

DI_MODULE_EXPORT
template<class T>
using ContextParameterOf = decltype(detail::getContextParameter(std::declval<T>()));

DI_MODULE_EXPORT
template<class T>
using ContextOf = decltype(detail::getContext<T>());

DI_MODULE_EXPORT
template<class T, class Context>
concept HasContext = std::same_as<Context, ContextOf<T>>;

DI_MODULE_EXPORT
template<IsContext Context>
using ContextToNode = Context::template NodeTmpl<detail::Decompress<Context>>;

DI_MODULE_EXPORT
template<IsContext Context>
using ContextToNodeState = detail::ToNodeState<ContextToNode<Context>>;

DI_MODULE_EXPORT
struct NullContext;

DI_MODULE_EXPORT
template<class Root>
struct RootContext;

DI_MODULE_EXPORT
template<class T>
concept IsRootContext = IsContext<T> and std::same_as<T, typename T::Root::Context>;

DI_MODULE_EXPORT
template<class Parent_, IsNodeHandle NodeHandle>
struct Context;

DI_MODULE_EXPORT
template<class Parent, template<class> class NodeTmpl>
using InlineContext = Context<Parent, InlineNode<NodeTmpl>>;

} // namespace di


#endif // INCLUDE_DI_CONTEXT_FWD_HPP
