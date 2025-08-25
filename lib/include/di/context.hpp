#ifndef INCLUDE_DI_CONTEXT_HPP
#define INCLUDE_DI_CONTEXT_HPP

#include "di/detail/as_ref.hpp"
#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"

#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#include "di/finalize.hpp"
#include "di/global_context.hpp"
#include "di/global_trait.hpp"
#include "di/key.hpp"
#include "di/link.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <tuple>
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct ContextBase
    {
        // Delegate to parent cluster to get the global node
        template<IsContext Self_, class N>
        constexpr auto& getGlobalNode(this Self_, N& node)
        {
            using Self = detail::Decompress<Self_>;
            auto memPtr = getNodePointer(AdlTag<Self>{});
            return typename Self::ParentContext{}.getGlobalNode(memPtr.getClassFromMember(node));
        }

        template<IsContext Self, IsGlobalTrait GlobalTrait>
        constexpr auto getNode(this Self self, auto& node, GlobalTrait)
        {
            detail::assertContextHasGlobalTrait<Self, GlobalTrait>();
            return self.getGlobalNode(node).asTrait(detail::AsRef{}, typename GlobalTrait::Trait{});
        }

        // Get sibling node
        template<IsContext Self_, IsTrait Trait>
        requires detail::HasLink<Self_, Trait>
        constexpr auto getNode(this Self_, auto& node, Trait)
        {
            using Self = detail::Decompress<Self_>;
            using Other = detail::ResolveLink<Self, Trait>;
            // Explicitly disallow "hairpin" dependencies, as they could subvert
            // dynamic access controls via re-entry into the same protected context
            // region hosted in a different instance. As a runtime optimisation, context
            // regions with dynamic access enforcement can elide access checks on getNode
            // calls targeting the identical protected context region. However, if the same
            // region can be re-entered in a single getNode call via a hairpin
            // dependency which at some point leaves the region, then the static context
            // region being identical does not necessarily mean that the dynamic context
            // is, and so checks cannot be elided. There is no legitimate reason for a
            // node to depend on itself; asTrait is available instead. Disallowing
            // depending on oneself allows for the runtime optimisation to exist.
            static_assert(not std::is_same_v<typename Other::Context, Self>, "Dependency on self not allowed");
            auto thisMemPtr = getNodePointer(AdlTag<Self>{});
            auto otherMemPtr = getNodePointer(AdlTag<typename Other::Context>{});
            auto& otherNode = otherMemPtr.getMemberFromClass(thisMemPtr.getClassFromMember(node));
            return otherNode.asTrait(detail::AsRef{}, typename Other::Trait{});
        }

        // Delegate to parent cluster to get the node
        template<IsContext Self_, IsTrait Trait>
        requires detail::LinksToParent<Self_, Trait>
        constexpr auto getNode(this Self_, auto& node, Trait)
        {
            using Self = detail::Decompress<Self_>;
            auto memPtr = getNodePointer(AdlTag<Self>{});
            return typename Self::ParentContext{}.getNode(memPtr.getClassFromMember(node), detail::ResolveLinkTrait<Self, Trait>{});
        }

        // Get member pointer from the perspective of a parent to this node
        template<IsContext Parent, IsContext Self_>
        constexpr auto getParentMemPtr(this Self_)
        {
            using Self = detail::Decompress<Self_>;
            auto memPtr = getNodePointer(AdlTag<Self>{});
            if constexpr (std::is_same_v<Self, detail::Decompress<Parent>>)
            {
                return memPtr;
            }
            else
            {
                auto parentMemPtr = typename Self::ParentContext{}.template getParentMemPtr<Parent>();
                return parentMemPtr + memPtr;
            }
        }

        // Get the reference to a parent node by its context
        template<IsContext Parent, IsContext Self_>
        constexpr auto& getParentNode(this Self_, auto& node)
        {
            using Self = detail::Decompress<Self_>;
            if constexpr (std::is_same_v<Self, detail::Decompress<Parent>>)
            {
                return node;
            }
            else
            {
                auto memPtr = getNodePointer(AdlTag<Self>{});
                return typename Self::ParentContext{}.template getParentNode<Parent>(memPtr.getClassFromMember(node));
            }
        }
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

        static void implicitDependencyAllowed() = delete;

        template<class Source, class Target, class Key = ContextOf<Source>::Info::DefaultKey>
        DI_INLINE static constexpr auto finalize(Source& source, Target& target, Key const& key = {}, auto const&... keys)
        {
            return di::finalize(source, target, key, keys...);
        }

        static constexpr void assertAccessible(auto&) {}

        template<class Source, class Target>
        static consteval std::tuple<> requiresKeysToTarget()
        {
            return {};
        }
    };

    static constexpr std::size_t Depth = 0;
};

template<class Root_>
struct RootContext : NullContext
{
    using Root = Root_;
};

template<class Parent_, IsNodeHandle NodeHandle>
struct Context : detail::ContextBase
{
    using Parent = Parent_;
    using ParentContext = ContextParameterOf<Parent>;
    using Root = ParentContext::Root;
    using Info = ParentContext::Info;

    static constexpr std::size_t Depth = 1 + ParentContext::Depth;

    template<std::derived_from<Context> Self>
    using NodeTmpl = ToNodeWrapper<NodeHandle>::template Node<detail::CompressContext<Self>>;
};

DI_MODULE_EXPORT
template<class Context, class Trait>
concept ContextHasTrait = IsContext<Context> and IsTrait<Trait> and HasTrait<ContextToNode<Context>, Trait>;

DI_MODULE_EXPORT
template<IsTrait Trait>
struct ContextHasTraitPred
{
    template<IsContext Context>
    static constexpr bool value = ContextHasTrait<Context, Trait>;
};

} // namespace di


#endif // INCLUDE_DI_CONTEXT_HPP
