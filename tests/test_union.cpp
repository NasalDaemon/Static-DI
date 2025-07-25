#include "di/macros.hpp"

#include <doctest/doctest.h>

#if !DI_IMPORT_STD
#include <concepts>
#include <cstdio>
#include <utility>
#endif

import di;
#if DI_IMPORT_STD
import std;
#endif

namespace di::tests::union_ {

namespace trait {

    struct Name : di::Trait
    {
        #define DI_METHODS_Name(TAG) \
            TAG(get) \

        DI_METHODS(Name)

        template<class Self, class T, class Types>
        requires requires (T const c)
        {
            { c.impl(get_c) } -> std::same_as<int>;
        }
        using Implements = void;
    };

} // trait

struct MouseType;

struct Cat : di::Node
{
    using Traits = di::Traits<Cat, trait::Name>;
    struct Types
    {
        struct CatType;
    };

    int impl(this auto& self, trait::Name::get)
    {
        return self.getNode(trait::Name{}).get() + 99;
    }
};
struct Dog
{
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node, trait::Name>;
        struct Types
        {
            struct DogType;
        };

        static_assert(std::is_same_v<MouseType, typename di::ResolveTypes<Node, trait::Name>::MouseType>);

        int impl(trait::Name::get) const
        {
            return getNode(trait::Name{}).get();
        }
    };
};
template<class Context>
struct Mouse : di::Node
{
    using Traits = di::Traits<Mouse, trait::Name>;
    struct Types
    {
        using MouseType = union_::MouseType;
    };

    int impl(trait::Name::get) const { return 42; }
};

template<class Context>
struct Union : di::Cluster
{
    struct Onion;
    struct Mouse;

    DI_LINK(trait::Name, Onion)

    struct Onion : di::Context<Union, di::Union<Cat, Dog>>
    {
        DI_LINK(trait::Name, Mouse)
    };
    struct Mouse : di::InlineContext<Union, union_::Mouse>
    {
        DI_LINK(trait::Name, Onion)
    };

    DI_NODE(Onion, onion)
    DI_NODE(Mouse, mouse)
};

constexpr bool testWithIndex()
{
    for (int i = 0; i < 24; ++i)
    {
        if (i != di::withIndex<24>(i, [](auto index) -> int { return index; }))
            return false;
    }
    return true;
}

static_assert(testWithIndex());

TEST_CASE("di::Union")
{
    di::InlineGraph<Union> cat{.onion{std::in_place_index<0>}};
    di::InlineGraph<Union> dog{.onion{di::withFactory, [](auto c) { return c(std::in_place_type<Dog>); }}};

    using DogTypes = decltype(dog.onion.asTrait(trait::Name{}))::Types;
    static_assert(DogTypes::TypesCount == 2);
    static_assert(requires { typename DogTypes::TypesAt<0>::CatType; });
    static_assert(requires { typename DogTypes::TypesAt<1>::DogType; });

    CHECK(42 == cat.mouse.asTrait(trait::Name{}).get());

    CHECK(141 == cat.onion.asTrait(trait::Name{}).get());

    CHECK(141 == cat.mouse.getNode(trait::Name{}).get());

    CHECK(42 == dog.mouse.asTrait(trait::Name{}).get());

    CHECK(42 == dog.mouse.getNode(trait::Name{}).get());

    CHECK(42 == dog.onion.asTrait(trait::Name{}).get());

    CHECK(cat.mouse.asTrait(trait::Name{})->visit([]<class T>(T) { return requires { typename T::Types::MouseType; }; }));

    CHECK(dog.mouse.asTrait(trait::Name{})->visit([]<class T>(T) { return requires { typename T::Types::MouseType; }; }));

    CHECK(cat.onion.asTrait(trait::Name{})->visit([]<class T>(T) { return requires { typename T::Types::CatType; }; }));

    CHECK(dog.onion.asTrait(trait::Name{})->visit([]<class T>(T) { return requires { typename T::Types::DogType; }; }));
}

} // di::tests::union_

DI_INSTANTIATE(di::InlineGraph<di::tests::union_::Union>, onion->get<1>())
