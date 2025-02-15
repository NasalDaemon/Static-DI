#ifndef INCLUDE_DI_TRAIT_VIEW_HPP
#define INCLUDE_DI_TRAIT_VIEW_HPP

#include "di/detail/concepts.hpp"
#include "di/detail/finalize.hpp"

#include "di/alias.hpp"
#include "di/key.hpp"
#include "di/trait.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/traits_fwd.hpp"

#if !DI_STD_MODULE
#include <functional>
#include <type_traits>
#endif

namespace di {

namespace detail {
    template<class T>
    inline constexpr bool isTraitView = false;
}

DI_MODULE_EXPORT
template<class T>
concept IsTraitView = detail::isTraitView<std::remove_cvref_t<T>>;

DI_MODULE_EXPORT
template<class T, class Trait, class Key = key::Default>
concept IsTraitViewOf = IsTraitView<T> and IsTrait<Trait> and std::derived_from<T, typename key::Trait<Key, Trait>::Meta::Methods>;

DI_MODULE_EXPORT
struct AsFunctor{} inline constexpr asFunctor;

DI_MODULE_EXPORT
template<class Trait, IsMethodOf<Trait> Method, Implements<Trait> Impl>
struct TraitMethodFunctor
{
    constexpr explicit TraitMethodFunctor(Alias<Impl> impl) : impl(impl) {}

    constexpr decltype(auto) operator()(this auto&& self, auto&&... args)
    {
        return self.impl->apply(Method{}, DI_FWD(args)...);
    }

private:
    Alias<Impl> impl;
};

#ifdef __INTELLISENSE__
DI_MODULE_EXPORT
template<IsTrait Trait>
struct IntellisenseTraitView final : Trait::Meta::Methods
{
    static consteval std::true_type isTrait(MatchesTrait<Trait> auto) { return {}; }
    static consteval std::false_type isTrait(auto) { return {}; }

    using Types = EmptyTypes;
    struct Traits
    {
        template<IsTrait T>
        using ResolveInterface = IntellisenseTraitView;

        template<IsTrait T>
        using ResolveTypes = Types;
    };

    template<IsMethodOf<Trait> Method>
    constexpr decltype(auto) apply(this auto&& self, Method trait, auto&&... args);

    constexpr decltype(auto) visit(this auto&& self, auto&& visitor);
};
#endif

// Presents a view over a trait implementation, where only the trait trait functions are accessible
DI_MODULE_EXPORT
template<IsTrait Trait, class Impl>
struct TraitView final : Trait::Meta::Methods
{
    constexpr TraitView(Trait, Impl& impl) : TraitView(Alias(impl)) {}

    static consteval std::true_type isTrait(MatchesTrait<Trait> auto) { return {}; }
    static consteval std::false_type isTrait(auto) { return {}; }

    using Types = NodeTypes<Impl, Trait>;
    struct Traits
    {
        template<IsTrait T>
        using ResolveInterface = TraitView;

        template<IsTrait T>
        using ResolveTypes = Types;
    };

    template<IsMethodOf<Trait> Method>
    constexpr decltype(auto) apply(this auto&& self, Method trait, auto&&... args)
    {
        return self.impl->apply(trait, DI_FWD(args)...);
    }

    template<IsMethodOf<Trait> Method>
    constexpr TraitMethodFunctor<Trait, Method, Impl> apply(this auto&& self, Method, di::AsFunctor)
    {
        return TraitMethodFunctor<Trait, Method, Impl>(self.impl);
    }

    // Visit the TraitView of the concrete implementation (e.g. active node of union)
    constexpr decltype(auto) visit(this auto&& self, auto&& visitor)
    {
        return self.impl->visit(
            [&](auto& impl)
            {
                return std::invoke(DI_FWD(visitor), impl.asTrait(Trait{}));
            });
    }

private:
    template<IsTrait Trait1, class Impl1>
    friend struct TraitView;

    constexpr TraitView(Alias<Impl> impl)
    : impl(impl)
    {
        static_assert(not IsTrait<TraitView>);
        DI_ASSERT_IMPLEMENTS(Impl, Trait);
        if constexpr (not std::is_const_v<Impl>)
            DI_ASSERT_IMPLEMENTS(TraitView, Trait);
    }

    Alias<Impl> impl;
};

DI_MODULE_EXPORT
template<IsTrait Trait, class Impl>
TraitView(Trait, Impl&) -> TraitView<Trait, Impl>;

namespace detail {
    template<class Trait, class Impl>
    constexpr bool isTraitView<TraitView<Trait, Impl>> = true;
#ifdef __INTELLISENSE__
    template<class Trait>
    constexpr bool isTraitView<IntellisenseTraitView<Trait>> = true;
#endif
}

DI_MODULE_EXPORT
template<class Trait, class Key>
constexpr IsTraitViewOf<Trait, Key> auto makeTraitView(auto& source, auto& target, Trait, Key key)
{
    return TraitView(key::Trait<Key, Trait>{}, detail::finalize(source, target, key));
}


} // namespace di


#endif // INCLUDE_DI_TRAIT_VIEW_HPP
