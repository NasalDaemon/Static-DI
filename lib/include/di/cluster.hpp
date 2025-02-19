#ifndef INCLUDE_DI_CLUSTER_HPP
#define INCLUDE_DI_CLUSTER_HPP

#include "di/context_fwd.hpp"
#include "di/key.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/trait.hpp"
#include "di/trait_view.hpp"

namespace di {

namespace detail {
    struct OnGraphConstructedVisitor
    {
        static constexpr void operator()(auto& node)
        {
            if constexpr (requires { node.onGraphConstructed(); })
                node.onGraphConstructed();
        }
    };
}

DI_MODULE_EXPORT
struct Cluster
{
    using Environment = di::Environment<>;

    template<class Self>
    requires IsRootContext<ContextParameterOf<Self>>
    void onConstructed(this Self& self)
    {
        self.visit(detail::OnGraphConstructedVisitor{});
    }

    template<class Self, IsTrait Trait, class Key = ContextParameterOf<Self>::Info::DefaultKey>
    constexpr IsTraitViewOf<Trait, Key> auto getNode(this Self& cluster, Trait trait, Key key = {})
    {
        auto& target = cluster.getNode(trait, key::Bypass{});
        return makeTraitView(cluster, target, trait, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto& getNode(this Self& cluster, Trait trait, key::Bypass)
    {
        return detail::getContextParameter(cluster).getNode(cluster, trait);
    }

    template<class Self, IsTrait Trait>
    constexpr auto canGetNode(this Self const&, Trait) -> std::bool_constant<
        requires (Self c, Trait trait) {
            detail::getContextParameter(c).getNode(c, trait);
        }>
    {
        return {};
    }

    template<class Self, IsTrait Trait, class Key = ContextParameterOf<Self>::Info::DefaultKey>
    requires detail::HasLink<Self, Trait>
    constexpr IsTraitViewOf<Trait, Key> auto asTrait(this Self& self, Trait trait, Key key = {})
    {
        auto& target = self.asTrait(trait, key::Bypass{});
        return makeTraitView(self, target, trait, key);
    }

    template<class Self, IsTrait Trait>
    requires detail::HasLink<Self, Trait>
    constexpr auto& asTrait(this Self& cluster, Trait, key::Bypass key)
    {
        using Target = detail::ResolveLink<Self, Trait>;
        auto& node = cluster.*getNodePointer(AdlTag<typename Target::Context>{});
        return node.asTrait(typename Target::Trait{}, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto hasTrait(this Self const&, Trait) -> std::bool_constant<
        detail::HasLink<Self, Trait> and
        requires (AdlTag<detail::ResolveLinkContext<Self, Trait>> adlTag) {
            getNodePointer(adlTag);
        }>
    {
        return {};
    }
};

DI_MODULE_EXPORT
template<class T>
concept IsCluster = std::derived_from<T, Cluster>;

} // namespace di


#endif // INCLUDE_DI_CLUSTER_HPP
