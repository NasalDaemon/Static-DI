#ifndef INCLUDE_DI_CONTEXT_HPP
#define INCLUDE_DI_CONTEXT_HPP

#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"

#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/context_fwd.hpp"

#include "di/environment.hpp"
#include "di/key.hpp"
#include "di/link.hpp"
#include "di/trait.hpp"
#include "di/trait_view.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct ContextBase
    {
        // Trait view of sibling node
        template<IsContext Self_, IsTrait Trait>
        requires detail::HasLink<detail::Decompress<Self_>, Trait>
        constexpr auto& getNode(this Self_, auto& node, Trait)
        {
            using Self = detail::Decompress<Self_>;
            using Other = detail::ResolveLink<Self, Trait>;
            auto& otherNode = getCluster<Self>(node).*getNodePointer(AdlTag<typename Other::Context>{});
            return otherNode.asTrait(typename Other::Trait{}, key::Bypass{});
        }

        // Delegate to parent cluster to get the trait view
        template<IsContext Self_, IsTrait Trait>
        requires detail::LinksToParent<detail::Decompress<Self_>, Trait>
        constexpr auto& getNode(this Self_, auto& node, Trait)
        {
            using Self = detail::Decompress<Self_>;
            return getCluster<Self>(node).getNode(detail::ResolveLinkTrait<Self, Trait>{}, key::Bypass{});
        }

    private:
        template<class Self>
        inline static auto& getCluster(auto& node)
        {
            constexpr auto memPtr = getNodePointer(AdlTag<Self>{});
            auto& n = detail::downCast<ContextToNodeState<Self>>(node);
            return n.*detail::reverseMemberPointer(memPtr);
        }
    };

    template<IsNode Node>
    struct NodeState : private Node
    {
        using Node::Node;

        // Expose utility functions from the underlying node
        using typename Node::Traits;
        using typename Node::Types;
        using typename Node::Environment;
        using Node::getNode;
        using Node::canGetNode;
        using Node::asTrait;
        using Node::hasTrait;

        constexpr void visit(this auto& self, auto const& f)
        {
            upCast<Node>(self).visit(f);
        }

        template<class Self>
        constexpr auto& get(this Self& self)
        {
            ContextOf<Self>::Info::template assertAccessible<typename Self::Environment>();
            return upCast<Node>(self);
        }

        constexpr auto* operator->(this auto& self) { return std::addressof(self.get()); }
    };
}

struct NullContext : detail::ContextBase
{
    // May not be overridden
    struct Root{};
    // May be overridden
    struct Info
    {
        using DefaultKey = key::Default;

        template<class Source, class Target, class Key>
        static constexpr auto& finalize(Source&, Target& target, Key)
        {
            using Env = Source::Environment;
            using WithEnv = di::WithEnv<Env, Target>;
            using FinalInterface = Key::template Interface<WithEnv>;
            return detail::downCast<FinalInterface>(target);
        }

        template<class Environment>
        static consteval void assertAccessible() {}
    };
};

template<class Parent_, IsNodeHandle NodeHandle>
struct Context : detail::ContextBase
{
    using Parent = Parent_;
    using ParentContext = ContextParameterOf<Parent>;
    using Root = ParentContext::Root;
    using Info = ParentContext::Info;

    template<std::derived_from<Context> Self>
    using NodeTmpl = ToNodeWrapper<NodeHandle>::template Node<detail::CompressContext<Self>>;
};

} // namespace di


#endif // INCLUDE_DI_CONTEXT_HPP
