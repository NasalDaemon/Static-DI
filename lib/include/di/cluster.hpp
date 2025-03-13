#ifndef INCLUDE_DI_CLUSTER_HPP
#define INCLUDE_DI_CLUSTER_HPP

#include "di/context_fwd.hpp"
#include "di/key.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/trait.hpp"
#include "di/trait_view.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct OnGraphConstructedVisitor
    {
        constexpr void operator()(auto& node) const
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
        auto& target = getNodeRef(cluster, trait, key::Bypass{});
        return makeTraitView(cluster, target, trait, key);
    }

    template<class Self, IsTrait Trait>
    friend constexpr auto& getNodeRef(Self& cluster, Trait trait, key::Bypass)
    {
        return detail::getContextParameter(cluster).getNode(cluster, trait);
    }

    template<class Self, IsTrait Trait>
    constexpr auto canGetNode(this Self const&, Trait)
    {
        constexpr bool value =
            requires (Self c, Trait trait) {
                detail::getContextParameter(c).getNode(c, trait);
            };
        return std::bool_constant<value>{};
    }

    template<class Self, IsTrait Trait, class Key = ContextParameterOf<Self>::Info::DefaultKey>
    requires detail::HasLink<Self, Trait>
    constexpr IsTraitViewOf<Trait, Key> auto asTrait(this Self& self, Trait trait, Key key = {})
    {
        auto& target = asTraitRef(self, trait, key::Bypass{});
        return makeTraitView(self, target, trait, key);
    }

    template<class Self, IsTrait Trait>
    requires detail::HasLink<Self, Trait>
    friend constexpr auto& asTraitRef(Self& cluster, Trait, key::Bypass key)
    {
        using Target = detail::ResolveLink<Self, Trait>;
        auto& node = cluster.*getNodePointer(AdlTag<typename Target::Context>{});
        return asTraitRef(node, typename Target::Trait{}, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto hasTrait(this Self const&, Trait)
    {
        constexpr bool value =
            detail::HasLink<Self, Trait> and
            requires (AdlTag<detail::ResolveLinkContext<Self, Trait>> adlTag) {
                getNodePointer(adlTag);
            };
        return std::bool_constant<value>{};
    }
};

DI_MODULE_EXPORT
template<class T>
concept IsCluster = std::derived_from<T, Cluster>;

} // namespace di


#endif // INCLUDE_DI_CLUSTER_HPP
