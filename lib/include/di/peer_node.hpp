#ifndef INCLUDE_DI_PEER_NODE_HPP
#define INCLUDE_DI_PEER_NODE_HPP

#include "di/detached.hpp"
#include "di/detail/cast.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/traits/peer.hxx"
#include "di/traits_fwd.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct PeerNode : Node
{
    // Also exposed in TraitNodeView
    template<class Self>
    requires IsElementContext<ContextOf<Self>>
    constexpr auto const& getElementId(this Self& self)
    {
        using ThisNode = detail::UnderlyingNode<Self>;
        auto& node = detail::upCast<ThisNode>(self);
        using ElementContext = ContextOf<Self>::Info::ElementContext;
        auto& element = ContextOf<Self>{}.template getParentNode<ElementContext>(node);
        auto memPtr = ElementContext{}.template getParentMemPtr<ElementContext>();
        return memPtr.getClassFromMember(element).id;
    }

    // Also exposed in TraitNodeView
    template<class Self>
    requires IsElementContext<ContextOf<Self>>
    constexpr auto getElementHandle(this Self const& self)
    {
        using ThisNode = detail::UnderlyingNode<Self>;
        auto& node = detail::upCast<ThisNode>(self);
        using ElementContext = ContextOf<Self>::Info::ElementContext;
        auto& element = ContextOf<Self>{}.template getParentNode<ElementContext>(node);
        auto memPtr = ElementContext{}.template getParentMemPtr<ElementContext>();
        return memPtr.getClassFromMember(element).getElementHandle();
    }

    // Only available to the node itself
    template<class Self>
    requires IsElementContext<ContextOf<Self>> and HasTrait<Self, trait::Peer>
    constexpr auto getPeers(this Self& self)
    {
        using ThisNode = detail::UnderlyingNode<Self>;
        auto& node = detail::upCast<ThisNode>(self);
        using ElementContext = ContextOf<Self>::Info::ElementContext;
        // Deliberately use getParentMemPtr as it only works for nodes with a stable memory location compared to the parent
        // Trying to allow peer access to nodes with dynamic context opens up a can of worms which is not worth it
        auto memPtr = ContextOf<Self>{}.template getParentMemPtr<ElementContext>();
        return memPtr.getClassFromMember(node).template getPeers<Self>(memPtr);
    }

    // Default impl of di::trait::Peer is to have no peers
    constexpr std::false_type impl(this auto const&, trait::Peer::isPeerId, auto const&) { return {}; }
    constexpr std::false_type impl(this auto const&, trait::Peer::isPeerInstance, auto const&) { return {}; }

    template<class Self>
    static constexpr void assertNodeContext()
    {
        static_assert(detail::TraitsHasTrait<typename Self::Traits, trait::Peer>, "Peer node is missing implementation for di::trait::Peer.");
        Node::assertNodeContext<Self>();
    }
};

DI_MODULE_EXPORT
struct PeerDetached : DetachedInterface
{
    // Default impl of di::trait::Peer is to accept no peers
    constexpr std::false_type impl(this auto const&, trait::Peer::isPeerId, auto const&) { return {}; }
    constexpr std::false_type impl(this auto const&, trait::Peer::isPeerInstance, auto const&) { return {}; }
};

DI_MODULE_EXPORT
struct PeerDetachedOpen : DetachedInterface
{
    // Default impl of di::trait::Peer is to accept all peers
    constexpr std::true_type impl(this auto const&, trait::Peer::isPeerId, auto const&) { return {}; }
    constexpr std::true_type impl(this auto const&, trait::Peer::isPeerInstance, auto const&) { return {}; }
};

}


#endif // INCLUDE_DI_PEER_NODE_HPP
