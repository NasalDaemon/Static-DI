#ifndef INCLUDE_DI_CONTEXT_HPP
#define INCLUDE_DI_CONTEXT_HPP

#include "di/detail/as_ref.hpp"
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

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct ContextBase
    {
        // Get sibling node
        template<IsContext Self_, IsTrait Trait>
        requires detail::HasLink<Self_, Trait>
        constexpr auto getNode(this Self_, auto& node, Trait)
        {
            using Self = detail::Decompress<Self_>;
            using Other = detail::ResolveLink<Self, Trait>;
            // Explicitly disallow "hairpin bend" dependencies, as it could subvert dynamic access controls
            // via re-entry into the same protected context region hosted in a different instance.
            // As a runtime optimisation, context regions with dynamic access enforcement elide access checks
            // on getNode calls accessing the same protected context region. However, if the same region can be
            // re-entered in a single getNode call via a hairpin dependency which at some point leaves the region,
            // then the static context region being identical does not necessarily mean that the dynamic context is,
            // and so checks cannot be elided.
            // There is no legitimate reason for a node to depend on oneself, asTrait is available instead.
            // Disallowing depending on oneself allows for the runtime optimisation to exist.
            static_assert(not std::is_same_v<typename Other::Context, Self>, "Dependency on self not allowed");
            auto memPtr = getNodePointer(AdlTag<Self>{});
            auto& otherNode = getParent(node, memPtr).*getNodePointer(AdlTag<typename Other::Context>{});
            return otherNode.asTrait(detail::AsRef{}, typename Other::Trait{});
        }

        // Delegate to parent cluster to get the node
        template<IsContext Self_, IsTrait Trait>
        requires detail::LinksToParent<Self_, Trait>
        constexpr auto getNode(this Self_, auto& node, Trait)
        {
            using Self = detail::Decompress<Self_>;
            auto memPtr = getNodePointer(AdlTag<Self>{});
            return typename Self::ParentContext{}.getNode(getParent(node, memPtr), detail::ResolveLinkTrait<Self, Trait>{});
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
                return combineMemberPointers(parentMemPtr, memPtr);
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

        template<class Source, class Target, class Key = ContextOf<Source>::Info::DefaultKey>
        static constexpr auto finalize(Source&, Target& target, Key const& = {}, auto const&... keys)
        {
            using Env = Source::Environment;
            using WithEnv = di::WithEnv<Env, Target>;
            using FinalInterface = Key::template Interface<WithEnv>;
            return makeAlias(detail::downCast<FinalInterface>(target), keys...);
        }

        static constexpr void assertAccessible(auto&) {}
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
