#pragma once

#include "di/manual/trait.hpp"

namespace abc::trait {

struct AliceRead : di::Trait
{
    #define AliceRead_DI_METHODS(TAG) \
        TAG(get) \

    DI_METHODS(AliceRead)

    template<class Self, class T, class Types>
    requires requires (T const c)
    {
        typename Types::AliceType;
        { c.apply(get_c) } -> std::same_as<int>;
    }
    using Implements = void;
} inline constexpr aliceRead{};

struct AliceWrite : di::Trait
{
    #define AliceWrite_DI_METHODS(TAG) \
        TAG(set) \

    DI_METHODS(AliceWrite)

    template<class Self, class T, class Types>
    requires requires (T t, int i)
    {
        typename Types::AliceType;
        t.apply(set_c, i);
    }
    using Implements = void;
} inline constexpr aliceWrite{};

struct Bob : di::Trait
{
    #define Bob_DI_METHODS(TAG) \
        TAG(get) \
        TAG(set) \

    DI_METHODS(Bob)

    template<class Self, class T, class Types>
    requires requires (T const c)
    {
        typename Types::BobType;
        { c.apply(trait::Bob::get_c) } -> std::same_as<int>;
    }
    using Implements = void;
} inline constexpr bob{};

struct Charlie : di::Trait
{
    #define Charlie_DI_METHODS(TAG) \
        TAG(get) \

    DI_METHODS(Charlie)

    template<class Self, class T, class Types>
    requires requires (T const c)
    {
        typename Types::CharlieType;
        { c.apply(trait::Charlie::get_c) } -> std::same_as<int>;
        // c.getCharlies();
    }
    using Implements = void;
} inline constexpr charlie{};

struct Ellie : di::Trait
{
    #define Ellie_DI_METHODS(TAG) \
        TAG(get) \

    DI_METHODS(Ellie)

    template<class Self, class T, class Types>
    requires requires (T const c)
    {
        { c.apply(trait::Ellie::get_c) } -> std::same_as<int>;
    }
    using Implements = void;
} inline constexpr ellie{};

struct Visitable : di::Trait
{
    #define Visitable_DI_METHODS(TAG) \
        TAG(count) \

    DI_METHODS(Visitable)

    template<class Self, class T, class Types>
    using Implements = void;
} inline constexpr visitable{};

using Alice = di::JoinedTrait<AliceRead, AliceWrite>;
using Charlie2 = di::AltTrait<Charlie, struct Charlie_Tag2>;
using Charlie3 = di::AltTrait<Charlie, struct Charlie_Tag3>;
using Ellie2 = di::AltTrait<Ellie, struct Ellie_Tag2>;
using Ellie3 = di::AltTrait<Ellie, struct Ellie_Tag3>;
inline constexpr Alice alice{};
inline constexpr Charlie2 charlie2{};
inline constexpr Charlie3 charlie3{};
inline constexpr Ellie2 ellie2{};
inline constexpr Ellie3 ellie3{};

}

#undef AliceRead_DI_METHODS
#undef AliceWrite_DI_METHODS
#undef Bob_DI_METHODS
#undef Charlie_DI_METHODS
