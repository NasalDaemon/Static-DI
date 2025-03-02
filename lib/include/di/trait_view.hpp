#ifndef INCLUDE_DI_TRAIT_VIEW_HPP
#define INCLUDE_DI_TRAIT_VIEW_HPP

#include "di/detail/concepts.hpp"

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
template<class Trait, IsMethodOf<Trait> Method, class AliasImpl>
struct TraitMethodFunctor
{
    constexpr explicit TraitMethodFunctor(AliasImpl impl) : impl(impl) {}

    constexpr decltype(auto) operator()(this auto&& self, auto&&... args)
    {
        return self.impl.get().apply(Method{}, DI_FWD(args)...);
    }

private:
    AliasImpl impl;
};

namespace detail {
    struct Unknown
    {
        template<class T>
        explicit(false) constexpr operator T&&();
        template<class T>
        explicit(false) constexpr operator T&();
    };
}

DI_MODULE_EXPORT
template<IsTrait Trait>
struct AutoCompleteTraitView final : Trait::Meta::Methods
{
    static consteval std::true_type isTrait(MatchesTrait<Trait> auto) { return {}; }
    static consteval std::false_type isTrait(auto) { return {}; }

    using Types = EmptyTypes;
    struct Traits
    {
        template<IsTrait T>
        using ResolveInterface = AutoCompleteTraitView;

        template<IsTrait T>
        using ResolveTypes = Types;
    };

    template<IsMethodOf<Trait> Method>
    constexpr detail::Unknown apply(this auto&&, Method, auto&&...);

    constexpr detail::Unknown visit(this auto&& self, auto&& visitor);
};

// Presents a view over a trait implementation, where only the trait trait functions are accessible
DI_MODULE_EXPORT
template<IsTrait Trait, class ImplAlias>
struct TraitView final : Trait::Meta::Methods
{
    constexpr TraitView(Trait, ImplAlias impl) : TraitView(impl) {}

    static consteval std::true_type isTrait(MatchesTrait<Trait> auto) { return {}; }
    static consteval std::false_type isTrait(auto) { return {}; }

    using Impl = ImplAlias::Impl;
    using Types = NodeTypes<Impl, Trait>;
    struct Traits
    {
        template<std::same_as<TraitView> = TraitView>
        using GetContext = ContextOf<Impl>;

        template<std::same_as<Trait> T>
        using ResolveInterface = TraitView;

        template<std::same_as<Trait> T>
        using ResolveTypes = Types;
    };

    template<IsMethodOf<Trait> Method>
    constexpr decltype(auto) apply(this auto&& self, Method trait, auto&&... args)
    {
        return self.impl.get().apply(trait, DI_FWD(args)...);
    }

    template<IsMethodOf<Trait> Method>
    constexpr TraitMethodFunctor<Trait, Method, ImplAlias> apply(this auto&& self, Method, di::AsFunctor)
    {
        return TraitMethodFunctor<Trait, Method, ImplAlias>(self.impl);
    }

    // Visit the TraitView of the concrete implementation (e.g. active node of union)
    constexpr decltype(auto) visit(this auto&& self, auto&& visitor)
    {
        return self.impl.get().visit(
            [&](auto& impl)
            {
                return std::invoke(DI_FWD(visitor), impl.asTrait(Trait{}));
            });
    }

private:
    template<IsTrait Trait1, class Impl1>
    friend struct TraitView;

    constexpr TraitView(ImplAlias impl)
    : impl(impl)
    {
        static_assert(not IsTrait<TraitView>);
        DI_ASSERT_IMPLEMENTS(typename ImplAlias::Interface, Trait);
        if constexpr (not std::is_const_v<Impl>)
            DI_ASSERT_IMPLEMENTS(TraitView, Trait);
    }

    ImplAlias impl;
};

DI_MODULE_EXPORT
template<IsTrait Trait, class ImplAlias>
TraitView(Trait, ImplAlias) -> TraitView<Trait, ImplAlias>;

namespace detail {
    template<class Trait, class Impl>
    constexpr bool isTraitView<TraitView<Trait, Impl>> = true;
    template<class Trait>
    constexpr bool isTraitView<AutoCompleteTraitView<Trait>> = true;
}

DI_MODULE_EXPORT
template<class Trait, class Key>
constexpr IsTraitViewOf<Trait, Key> auto makeTraitView(auto& source, auto& target, Trait, Key key)
{
    return TraitView(key::Trait<Key, Trait>{}, target.finalize(source, key));
}


} // namespace di


#endif // INCLUDE_DI_TRAIT_VIEW_HPP
