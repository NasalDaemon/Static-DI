#ifndef INCLUDE_DI_CONTEXT_FWD_HPP
#define INCLUDE_DI_CONTEXT_FWD_HPP

#include "di/detail/concepts.hpp"

#include "di/key.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_STD_MODULE
#include <concepts>
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct ContextBase;
}

DI_MODULE_EXPORT
template<class T>
concept IsContext = std::is_base_of_v<detail::ContextBase, T> and std::is_empty_v<T> and requires {
    typename T::Root;
    typename T::Info;
};

namespace detail {
    template<template<class> class NodeTmpl, IsContext Context>
    constexpr Context getContextParameter(NodeTmpl<Context> const&)
    {
        return {};
    }
}

DI_MODULE_EXPORT
template<class T>
using ContextParameterOf = decltype(detail::getContextParameter(std::declval<T>()));

DI_MODULE_EXPORT
template<class T>
using ContextOf = T::Traits::template GetContext<>;

DI_MODULE_EXPORT
template<IsContext Context>
using ContextToNode = Context::template NodeTmpl<Context>;

namespace detail {
    template<IsNode Node>
    struct NodeState;

    template<IsNode T>
    auto nodeState() -> NodeState<T>;
    template<class T>
    auto nodeState() -> T;

    template<class T>
    void isNodeState(NodeState<T> const&);
    template<class T>
    concept IsNodeState = requires { detail::isNodeState(std::declval<T const&>()); };
}

DI_MODULE_EXPORT
template<IsContext Context>
using ContextToNodeState = decltype(detail::nodeState<ContextToNode<Context>>());

DI_MODULE_EXPORT
struct NullContext;

DI_MODULE_EXPORT
template<class Root>
struct RootContext;

DI_MODULE_EXPORT
template<class T>
concept IsRootContext = IsContext<T> and not requires { typename T::Parent; };

DI_MODULE_EXPORT
template<class Parent_, IsNodeHandle NodeHandle>
struct Context;

DI_MODULE_EXPORT
template<class Parent, template<class> class NodeTmpl>
using InlineContext = Context<Parent, InlineNode<NodeTmpl>>;

} // namespace di


#endif // INCLUDE_DI_CONTEXT_FWD_HPP
