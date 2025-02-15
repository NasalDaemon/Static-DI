#ifndef INCLUDE_DI_NODE_HPP
#define INCLUDE_DI_NODE_HPP

#include "di/detail/cast.hpp"
#include "di/detail/concepts.hpp"

#include "di/environment.hpp"
#include "di/key.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/trait.hpp"
#include "di/trait_view.hpp"
#include "di/traits_fwd.hpp"

#if !DI_STD_MODULE
#include <functional>
#include <type_traits>
#include <utility>
#endif

namespace di {

struct Node
{
    using Types = EmptyTypes;
    using Environment = di::Environment<>;

    template<class Self, std::invocable<Self&> Visitor>
    constexpr decltype(auto) visit(this Self& self, Visitor&& visitor)
    {
        return std::invoke(DI_FWD(visitor), self);
    }

#ifdef __INTELLISENSE__
    template<IsTrait Trait, class Key = key::Default>
    static constexpr IntellisenseTraitView<key::Trait<Key, Trait>> getNode(Trait trait, Key key = {});
#else
    template<class Self, IsTrait Trait, class Key = ContextOf<Self>::Info::DefaultKey>
    constexpr IsTraitViewOf<Trait, Key> auto getNode(this Self& self, Trait trait, Key key = {})
    {
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);
        auto& target = ContextOf<Self>{}.getNode(node, trait);
        return makeTraitView(self, target, trait, key);
    }
#endif

    template<class Self, IsTrait Trait>
    constexpr auto canGetNode(this Self&, Trait) -> std::bool_constant<
        requires (ContextOf<Self> c, Self::Traits::Node n, Trait trait) {
            c.getNode(n, trait);
        }>
    {
        return {};
    }

    template<class Self, IsTrait Trait, class Key = ContextOf<Self>::Info::DefaultKey>
    constexpr IsTraitViewOf<Trait, Key> auto asTrait(this Self& self, Trait trait, Key key = {})
    {
        auto& impl = self.asTrait(trait, key::Bypass{});
        return makeTraitView(self, impl, trait, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto& asTrait(this Self& self, Trait, key::Bypass)
    {
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);

        static_assert(Self::Traits::template HasTrait<Trait>, "Missing trait");
        using Interface = Self::Traits::template ResolveInterface<Trait>;
        return detail::downCast<Interface>(node);
    }

    template<class Self>
    constexpr auto& finalize(this Self& self, auto& source, auto key)
    {
        return ContextOf<Self>::Info::finalize(source, self, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto hasTrait(this Self&, Trait) -> std::bool_constant<detail::TraitsHasTrait<typename Self::Traits, Trait>>
    {
        return {};
    }
};

template<IsNode NodeT>
struct WrapNode
{
    template<class Context>
    using Node = WrappedImpl<NodeT, Context>;

    template<class Context>
    struct GetTraits
    {
        template<std::same_as<NodeT> T>
        using GetContext = Context;

        template<class Trait>
        requires detail::TraitsHasTrait<typename NodeT::Traits, Trait>
        struct Resolver
        {
            using Types = NodeT::Traits::template ResolveTypes<Trait>;
            using Interface = WrappedImpl<typename NodeT::Traits::template ResolveInterface<Trait>, Context>;
        };

        using Traits = detail::Traits<NodeT, GetContext, detail::TraitsTemplateDefault<Resolver>>;
    };

    template<class Context>
    using Traits = GetTraits<Context>::Traits;
};

} // namespace di


#endif // INCLUDE_DI_NODE_HPP
