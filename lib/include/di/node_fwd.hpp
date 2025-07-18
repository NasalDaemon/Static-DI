#ifndef INCLUDE_DI_NODE_FWD_HPP
#define INCLUDE_DI_NODE_FWD_HPP

#include "di/detail/concepts.hpp"
#include "di/factory.hpp"
#include "di/macros.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct Node;

DI_MODULE_EXPORT
struct PeerNode;

DI_MODULE_EXPORT
struct PeerNodeOpen;

DI_MODULE_EXPORT
template<class T>
concept IsNode = std::derived_from<T, Node>;

DI_MODULE_EXPORT
template<class T>
concept IsNodeWrapper = requires {
    typename detail::TakesUnaryClassTemplate<T::template Node>;
} and std::is_empty_v<T>;

DI_MODULE_EXPORT
template<IsNode NodeT>
struct WrapNode;

template<class Interface, class Context>
struct WrappedImpl : Interface
{
    using Interface::Interface;

    template<class F>
    explicit constexpr WrappedImpl(Emplace<F> const& f)
        : Interface(f)
    {}

    using Traits = WrapNode<typename Interface::Traits::Node>::template Traits<Context>;
};

namespace detail {
    template<class Trait, class Context>
    void isWrappedImpl(WrappedImpl<Trait, Context> const&);
}

DI_MODULE_EXPORT
template<class T>
concept IsWrappedImpl = requires (T const& t) { detail::isWrappedImpl(t); };

DI_MODULE_EXPORT
template<class T>
concept IsNodeHandle = IsNodeWrapper<T> or IsNode<T>;

namespace detail {
    template<IsNodeWrapper T>
    auto toNodeWrapper() -> T;
    template<IsNode T>
    auto toNodeWrapper() -> WrapNode<T>;
}

DI_MODULE_EXPORT
template<IsNodeHandle T>
using ToNodeWrapper = decltype(detail::toNodeWrapper<T>());

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

    template<class T>
    using ToNodeState = decltype(nodeState<T>());
}

DI_MODULE_EXPORT
template<template<class> class NodeTmpl>
struct InlineNode
{
    template<class Context>
    using Node = NodeTmpl<Context>;
};

DI_MODULE_EXPORT
template<class Node>
concept NodeHasDepends = Node::Depends::isSpecified;

DI_MODULE_EXPORT
template<class Node, class Trait>
concept NodeDependencyAllowed = Node::Depends::template dependencyAllowed<Trait>;

DI_MODULE_EXPORT
template<class Node, bool Transitive = false>
concept NodeDependenciesSatisfied = requires { typename Node::Depends::template AssertSatisfied<Node, Transitive>; };

namespace detail {

template<class Node>
struct NodeHasTraitsNodePred
{
    template<class T>
    static constexpr bool value = std::is_same_v<typename T::Traits::Node, Node>;
};

}

} // namespace di


#endif // INCLUDE_DI_NODE_FWD_HPP
