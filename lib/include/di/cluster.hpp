#ifndef INCLUDE_DI_CLUSTER_HPP
#define INCLUDE_DI_CLUSTER_HPP

#include "di/detail/as_ref.hpp"
#include "di/context_fwd.hpp"
#include "di/environment.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/requires.hpp"
#include "di/trait.hpp"
#include "di/trait_view.hpp"

#if !DI_IMPORT_STD
#include <compare>
#include <type_traits>
#endif

namespace di {

namespace detail {
    struct OnGraphConstructedVisitor
    {
        constexpr void operator()(IsNode auto& node) const
        {
            if constexpr (requires { node.onGraphConstructed(); })
                node.onGraphConstructed();
        }
    };
}

DI_MODULE_EXPORT
struct Cluster
{
    static constexpr bool isUnary() { return false; }
    using Environment = di::Environment<>;
    using Requires = di::Requires<>;

    template<class Self>
    requires IsRootContext<ContextParameterOf<Self>>
    void onConstructed(this Self& self)
    {
        self.visit(detail::OnGraphConstructedVisitor{});
    }

    template<class Self, IsTrait Trait>
    requires IsRootContext<ContextParameterOf<Self>>
    void visitTrait(this Self& self, Trait, auto&& visitor)
    {
        self.visit(
            [&visitor]<IsNode Node>(Node& node)
            {
                if constexpr (HasTrait<Node, Trait>)
                    node.asTrait(Trait{}).visit(visitor);
            });
    }

    template<class Self, IsTrait Trait, class Key = ContextParameterOf<Self>::Info::DefaultKey>
    constexpr IsTraitViewOf<Trait, Key> auto getNode(this Self& cluster, Trait trait, Key key = {})
    {
        auto target = cluster.getNode(detail::AsRef{}, trait);
        return makeTraitView(cluster, target, trait, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto getNode(this Self& cluster, detail::AsRef, Trait trait)
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
        auto target = self.asTrait(detail::AsRef{}, trait);
        return makeTraitView(self, target, trait, key);
    }

    template<class Self, IsTrait Trait>
    requires detail::HasLink<Self, Trait>
    constexpr auto asTrait(this Self& cluster, detail::AsRef asRef, Trait)
    {
        Self::template ensureDepth<ContextParameterOf<Self>>();
        using Target = detail::ResolveLink<Self, Trait>;
        auto& node = cluster.*getNodePointer(AdlTag<typename Target::Context>{});
        return node.asTrait(asRef, typename Target::Trait{});
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

    template<class Context>
    static consteval void ensureDepth() {}
};

DI_MODULE_EXPORT
struct DomainParams
{
    std::size_t MaxDepth = 3;

#if !DI_COMPILER_MSVC
    auto operator<=>(DomainParams const&) const = default;
#endif
};

DI_MODULE_EXPORT
template<DomainParams Params = {}>
struct Domain : Cluster
{
    template<class Context>
    static consteval void ensureDepth()
    {
        static_assert(Context::Depth <= Params.MaxDepth);
    }
};

DI_MODULE_EXPORT
template<class T>
concept IsCluster = std::derived_from<T, Cluster>;

} // namespace di


#endif // INCLUDE_DI_CLUSTER_HPP
