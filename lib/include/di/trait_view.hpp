#ifndef INCLUDE_DI_TRAIT_VIEW_HPP
#define INCLUDE_DI_TRAIT_VIEW_HPP

#include "di/detail/concepts.hpp"

#include "di/alias.hpp"
#include "di/key.hpp"
#include "di/trait.hpp"
#include "di/macros.hpp"
#include "di/traits_fwd.hpp"
#include "di/empty_types.hpp"

#if !DI_IMPORT_STD
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
    explicit constexpr TraitMethodFunctor(AliasImpl impl) : impl(impl) {}

    DI_INLINE constexpr decltype(auto) operator()(this auto&& self, auto&&... args)
    {
        return self.impl->impl(Method{}, DI_FWD(args)...);
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
    constexpr detail::Unknown impl(this auto&&, Method, auto&&...);

    constexpr detail::Unknown visit(this auto&& self, auto&& visitor);
};

// Presents a view over a trait implementation, where only the trait trait functions are accessible
DI_MODULE_EXPORT
template<IsTrait Trait, class ImplAlias, class Types_ = EmptyTypes>
struct TraitView final : Trait::Meta::Methods
{
    constexpr TraitView(Trait, ImplAlias alias, std::type_identity<Types_>)
        : alias(alias)
    {
        static_assert(not IsTrait<TraitView>);
        DI_ASSERT_IMPLEMENTS(typename ImplAlias::Interface, Types, Trait);
        if constexpr (not std::is_const_v<typename ImplAlias::Impl>)
            DI_ASSERT_IMPLEMENTS(TraitView, Types, Trait);
    }

    static consteval std::true_type isTrait(MatchesTrait<Trait> auto) { return {}; }
    static consteval std::false_type isTrait(auto) { return {}; }

    using Types = detail::Decompress<Types_>;
    struct Traits
    {
        using Node = ImplAlias::Impl;

        template<std::same_as<TraitView> = TraitView>
        using GetContext = ContextOf<typename ImplAlias::Impl>;

        template<std::same_as<Trait> T>
        using ResolveInterface = TraitView;

        template<std::same_as<Trait> T>
        using ResolveTypes = Types;
    };

    template<IsMethodOf<Trait> Method>
    DI_INLINE constexpr decltype(auto) impl(this auto&& self, Method trait, auto&&... args)
    {
        return self.alias->impl(trait, DI_FWD(args)...);
    }

    template<IsMethodOf<Trait> Method>
    DI_INLINE constexpr TraitMethodFunctor<Trait, Method, ImplAlias> impl(this auto&& self, Method, di::AsFunctor)
    {
        return TraitMethodFunctor<Trait, Method, ImplAlias>(self.alias);
    }

    // Visit the TraitView of the concrete implementation (e.g. active node of union)
    template<class Visitor>
    constexpr decltype(auto) visit(this auto&& self, Visitor&& visitor)
    {
        return self.alias->visit(
            [&](auto& impl) -> decltype(auto)
            {
                return std::invoke(DI_FWD(Visitor, visitor), impl.asTrait(Trait{}));
            });
    }

private:
    ImplAlias alias;
};

DI_MODULE_EXPORT
template<IsTrait Trait, class ImplAlias, class Types>
TraitView(Trait, ImplAlias, std::type_identity<Types>) -> TraitView<Trait, ImplAlias, Types>;

namespace detail {
    template<class Trait, class Impl, class Types>
    constexpr bool isTraitView<TraitView<Trait, Impl, Types>> = true;
    template<class Trait>
    constexpr bool isTraitView<AutoCompleteTraitView<Trait>> = true;
}

DI_MODULE_EXPORT
template<IsTrait Trait, key::IsKey Key>
constexpr IsTraitViewOf<Trait, Key> auto makeTraitView(auto& source, auto target, Trait, Key key)
{
    return TraitView(key::Trait<Key, Trait>{}, target.ptr->finalize(source, key), target.types());
}


} // namespace di


#endif // INCLUDE_DI_TRAIT_VIEW_HPP
