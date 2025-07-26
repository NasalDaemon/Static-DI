#ifndef INCLUDE_DI_NODE_HPP
#define INCLUDE_DI_NODE_HPP

#include "di/detail/as_ref.hpp"
#include "di/detail/cast.hpp"

#include "di/context_fwd.hpp"
#include "di/depends.hpp"
#include "di/empty_types.hpp"
#include "di/environment.hpp"
#include "di/factory.hpp"
#include "di/key.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/trait.hpp"
#include "di/trait_view.hpp"
#include "di/traits_fwd.hpp"

#if !DI_IMPORT_STD
#include <functional>
#include <type_traits>
#endif

namespace di {

struct Node
{
    static constexpr bool isUnary() { return true; }
    using Types = EmptyTypes;
    using Environment = di::Environment<>;
    using Depends = detail::DependsImplicitly;

    // Also exposed in TraitNodeView
    template<class Self, std::invocable<Self&> Visitor>
    DI_INLINE constexpr decltype(auto) visit(this Self& self, Visitor&& visitor)
    {
        return std::invoke(DI_FWD(visitor), self);
    }

    DI_INLINE constexpr auto& getState(this auto& self) { return self; }

#if DI_AUTOCOMPLETE
    template<IsTrait Trait, class Key = key::Default>
    static constexpr AutoCompleteTraitView<key::Trait<Key, Trait>> getNode(Trait trait, Key key = {});
#else
    template<class Self, IsTrait Trait, class Key = ContextOf<Self>::Info::DefaultKey>
    constexpr IsTraitViewOf<Trait, Key> auto getNode(this Self& self, Trait trait, Key key = {})
    {
        assertNodeDependencies<Self>();
        if constexpr (not detail::IsNodeState<Self>)
            static_assert(NodeDependencyAllowed<Self, Trait>, "Requested trait not listed in node definition");
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);
        auto target = ContextOf<Self>{}.getNode(node, trait);
        return makeTraitView(self, target, trait, key);
    }
#endif

    template<class Self, IsTrait Trait>
    constexpr auto canGetNode(this Self&, Trait)
    {
        constexpr bool value =
            requires (ContextOf<Self> c, Self::Traits::Node n, Trait trait) {
                c.getNode(n, trait);
            };
        return std::bool_constant<value>{};
    }

    template<class Self, IsTrait Trait, class Key = ContextOf<Self>::Info::DefaultKey>
    constexpr IsTraitViewOf<Trait, Key> auto asTrait(this Self& self, Trait trait, Key key = {})
    {
        auto impl = self.asTrait(detail::AsRef{}, trait);
        return makeTraitView(self, impl, trait, key);
    }

    template<class Self, IsTrait Trait>
    constexpr auto asTrait(this Self& self, detail::AsRef, Trait)
    {
        assertNodeDependencies<Self>();
        using ThisNode = Self::Traits::Node;
        auto& node = detail::upCast<ThisNode>(self);

        static_assert(Self::Traits::template HasTrait<Trait>, "Missing trait");
        using Interface = Self::Traits::template ResolveInterface<Trait>;
        using Types = Self::Traits::template ResolveTypes<Trait>;
        return detail::TargetRef{detail::downCast<Interface>(node), std::type_identity<Types>{}};
    }

    template<class Self, IsTrait Trait>
    constexpr auto hasTrait(this Self&, Trait) -> std::bool_constant<detail::TraitsHasTrait<typename Self::Traits, Trait>>
    {
        return {};
    }

    template<class Self>
    constexpr auto finalize(this Self& self, auto& source, auto key)
    {
        return ContextOf<Self>::Info::finalize(source, self, key);
    }

    template<class Self>
    static consteval void assertNodeDependencies()
    {
        static_assert(NodeDependenciesSatisfied<Self, true>, "Listed node dependencies are not satisfied transitively.");
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

namespace detail {

    template<IsNode Node>
    struct NodeState : private Node
    {
        using Node::Node;

        template<class F>
        explicit constexpr NodeState(Emplace<F> const& f)
            : Node(f)
        {}

        // Expose utility functions from the underlying node
        using Traits = Node::Traits;
        using Depends = Node::Depends;
        using Environment = Node::Environment;
        using Node::isUnary;
        using Node::getNode;
        using Node::canGetNode;
        using Node::asTrait;
        using Node::hasTrait;

        DI_INLINE constexpr decltype(auto) visit(this auto& self, auto&& f)
        {
            return upCast<Node>(self).visit(DI_FWD(f));
        }

        template<class Self>
        DI_INLINE constexpr auto& getState(this Self& self)
        {
            ContextOf<Self>::Info::template assertAccessible<typename Self::Environment>();
            return upCast<Node>(self).getState();
        }

        constexpr auto* operator->(this auto& self) { return std::addressof(self.getState()); }
    };

} // namespace detail

} // namespace di


#endif // INCLUDE_DI_NODE_HPP
