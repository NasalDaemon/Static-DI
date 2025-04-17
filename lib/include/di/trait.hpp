#ifndef INCLUDE_DI_TRAIT_HPP
#define INCLUDE_DI_TRAIT_HPP

#include "di/detail/compress.hpp"
#include "di/macros.hpp"

#if !DI_STD_MODULE
#include <concepts>
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
template<class... Ts>
struct AdlTag{};

DI_MODULE_EXPORT
struct Trait
{
    struct Meta
    {
        struct Applicable{};
        struct Methods{};
        struct DuckMethods{};
    };

    template<class Self, std::same_as<Self> Expected>
    void canProvide(this Self, Expected);

    template<class Self>
    Self expects(this Self);
};

DI_MODULE_EXPORT
struct UnconstrainedTrait : Trait
{
    template<class...>
    using Implements = void;
};

DI_MODULE_EXPORT
template<class T>
concept IsTrait = requires (T trait) {
    requires std::is_base_of_v<Trait, T>;
    requires std::is_empty_v<T>;
    std::is_empty_v<typename T::Meta>;
    std::is_empty_v<typename T::Meta::Applicable>;
    std::is_empty_v<typename T::Meta::Methods>;
    std::is_empty_v<typename T::Meta::DuckMethods>;
    trait.expects();
};

DI_MODULE_EXPORT
template<IsTrait Trait>
using TraitExpects = decltype(std::declval<Trait>().expects());

DI_MODULE_EXPORT
template<class Trait, class Expected>
concept TraitCanProvide = requires (Trait trait, Expected expected) {
    requires IsTrait<Trait>;
    requires IsTrait<Expected>;
    trait.canProvide(expected);
};

DI_MODULE_EXPORT
template<class T, class Trait>
concept MatchesTrait = TraitCanProvide<Trait, TraitExpects<T>>;

DI_MODULE_EXPORT
template<class T, class Trait>
concept ExactlyMatchesTrait = std::same_as<T, Trait> and MatchesTrait<T, Trait>;

namespace detail {
    template<class Trait, class Impl, class Types, IsTrait... Subtraits>
    requires (sizeof...(Subtraits) > 0)
    using ImplementsAll = Void<typename Subtraits::template Implements<Trait, Decompress<Impl>, Decompress<Types>>...>;

    template<class Impl, class Types, IsTrait... Traits>
    requires (sizeof...(Traits) > 0)
    using Implements = Void<typename Traits::template Implements<Traits, Decompress<Impl>, Decompress<Types>>...>;
}

DI_MODULE_EXPORT
template<class Impl, class Types, class... Traits>
concept Implements = requires { typename detail::Implements<Impl, Types, Traits...>; };

DI_MODULE_EXPORT
template<class Method, class Trait>
concept IsMethodOf = IsTrait<Trait> and requires (Method method, Trait::Meta::Applicable meta) {
    meta.applicable(method);
};

DI_MODULE_EXPORT
template<class Method>
using TraitOf = decltype(traitOf(std::declval<Method>()));

DI_MODULE_EXPORT
template<IsTrait... Traits>
requires (sizeof...(Traits) > 0)
struct JoinedTrait : Traits...
{
    struct Meta
    {
        struct Applicable : Traits::Meta::Applicable...
        {
            using Traits::Meta::Applicable::applicable...;
        };
        struct Methods : Traits::Meta::Methods...
        {};
        struct DuckMethods : Traits::Meta::DuckMethods...
        {};
    };

    template<class Self, class Impl, class Types>
    using Implements = detail::ImplementsAll<Self, Impl, Types, Traits...>;

    template<class Expected>
    requires (... or TraitCanProvide<Traits, Expected>)
    static void canProvide(Expected);

    static void canProvide(JoinedTrait);

    static JoinedTrait expects();
};

DI_MODULE_EXPORT
template<IsTrait Trait, class Id = decltype([]() -> void {})>
struct AltTrait : Trait
{
    static AltTrait expects();

    static void canProvide(AltTrait);
};

namespace detail {
    template<IsTrait Trait_>
    struct DuckTrait : di::Trait
    {
        struct Meta
        {
            using Applicable = Trait_::Meta::Applicable;
            using Methods = Trait_::Meta::DuckMethods;
            using DuckMethods = Trait_::Meta::DuckMethods;
        };

        static TraitExpects<Trait_> expects();

        template<class Self, class...>
        using Implements = void;
    };
}

}


#endif // INCLUDE_DI_TRAIT_HPP
