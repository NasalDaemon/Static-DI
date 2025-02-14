#pragma once

#include "di/manual/trait.hpp"

namespace abc::trait {

struct AliceRead : di::Trait
{
    #define ALICE_READ_METHODS(TAG) \
        TAG(get) \

    DI_METHODS(AliceRead, ALICE_READ_METHODS)

    template<class T, class Types = di::NodeTypes<T, trait::AliceRead>>
    requires requires (T const c)
    {
        typename Types::AliceType;
        { c.apply(get_c) } -> std::same_as<int>;
    }
    using Implements = void;
} inline constexpr aliceRead{};
struct AliceWrite : di::Trait
{
    #define ALICE_WRITE_METHODS(TAG) \
        TAG(set) \

    DI_METHODS(AliceWrite, ALICE_WRITE_METHODS)

    template<class T, class Types = di::NodeTypes<T, trait::AliceWrite>>
    requires requires (T t, int i)
    {
        typename Types::AliceType;
        t.apply(set_c, i);
    }
    using Implements = void;
} inline constexpr aliceWrite{};

using Alice = di::JoinedTrait<AliceRead, AliceWrite>;
inline constexpr Alice alice{};

struct Bob : di::Trait
{
    #define BOB_METHODS(TAG) \
        TAG(get) \
        TAG(set) \

    DI_METHODS(Bob, BOB_METHODS)

    template<class T, class Types = di::NodeTypes<T, Bob>>
    requires requires (T const c)
    {
        typename Types::BobType;
        { c.apply(trait::Bob::get_c) } -> std::same_as<int>;
    }
    using Implements = void;
} inline constexpr bob{};

struct Charlie : di::Trait
{
    #define CHARLIE_METHODS(TAG) \
        TAG(get) \

    DI_METHODS(Charlie, CHARLIE_METHODS)

    template<class T, class Types = di::NodeTypes<T, trait::Charlie>>
    requires requires (T const c)
    {
        typename Types::CharlieType;
        { c.apply(trait::Charlie::get_c) } -> std::same_as<int>;
        // c.getCharlies();
    }
    using Implements = void;
} inline constexpr charlie{};

using Charlie2 = di::AltTrait<Charlie>;
using Charlie3 = di::AltTrait<Charlie>;
inline constexpr Charlie2 charlie2{};
inline constexpr Charlie3 charlie3{};

}
