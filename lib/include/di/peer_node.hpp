#ifndef INCLUDE_DI_PEER_NODE_HPP
#define INCLUDE_DI_PEER_NODE_HPP

#include "di/detail/cast.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/peer.hxx"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct PeerNode : Node
{
    // Also exposed in TraitNodeView
    template<class Self>
    requires IsCollectionContext<ContextOf<Self>>
    constexpr auto const& getElementId(this Self& self)
    {
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);
        using CollectionContext = ContextOf<Self>::Info::CollectionContext;
        return detail::getParent(node, ContextOf<Self>{}.template getParentMemPtr<CollectionContext>()).id;
    }

    // Also exposed in TraitNodeView
    template<class Self>
    requires IsCollectionContext<ContextOf<Self>>
    constexpr auto getElementHandle(this Self const& self)
    {
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);
        using CollectionContext = ContextOf<Self>::Info::CollectionContext;
        return detail::getParent(node, ContextOf<Self>{}.template getParentMemPtr<CollectionContext>()).getElementHandle();
    }

    // Only available to the node itself
    template<class Self>
    requires IsCollectionContext<ContextOf<Self>> and HasTrait<Self, trait::Peer>
    constexpr auto getPeers(this Self& self)
    {
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);
        using CollectionContext = ContextOf<Self>::Info::CollectionContext;
        auto memPtr = ContextOf<Self>{}.template getParentMemPtr<CollectionContext>();
        return detail::getParent(node, memPtr).template getPeers<Self>(memPtr);
    }

    // Default impl of di::trait::Peer is to have no peers
    constexpr std::false_type impl(this auto const&, trait::Peer::isPeerId) { return {}; }
    template<class Self>
    constexpr std::false_type impl(this Self const&, trait::Peer::isPeerInstance, Self const&) { return {}; }
};

DI_MODULE_EXPORT
struct PeerNodeOpen : PeerNode
{
    // Default impl of di::trait::Peer is to accept all peers
    constexpr std::true_type impl(this auto const&, trait::Peer::isPeerId) { return {}; }
    template<class Self>
    constexpr std::true_type impl(this Self const&, trait::Peer::isPeerInstance, Self const&) { return {}; }
};

}


#endif // INCLUDE_DI_PEER_NODE_HPP
