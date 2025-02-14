#include "di/macros.hpp"
#if !DI_STD_MODULE
#include <concepts>
#include <cstdio>
#endif

import di;
DI_IMPORT_STD;

namespace test {

namespace trait {

    struct Name : di::Trait
    {
        #define NAME_METHODS(TAG) \
            TAG(get) \

        DI_METHODS(Name, NAME_METHODS)

        template<class T>
        requires requires (T const c)
        {
            { c.apply(get_c) } -> std::same_as<int>;
        }
        using Implements = void;
    };

} // trait

struct Cat : di::Node
{
    using Traits = di::Traits<Cat, trait::Name>;
    struct Types
    {
        struct CatType;
    };

    int apply(trait::Name::get) const { return 99; }
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

        int apply(trait::Name::get) const { return getNode(trait::Name{}).get(); }
    };
};
template<class Context>
struct Mouse : di::Node
{
    using Traits = di::Traits<Mouse, trait::Name>;
    struct Types
    {
        struct MouseType;
    };

    int apply(trait::Name::get) const { return 42; }
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
    struct Mouse : di::InlineContext<Union, test::Mouse>
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

inline bool test()
{
    di::InlineGraph<Union> cat{.onion{0u}};
    di::InlineGraph<Union> dog{.onion{1u}};

    using DogTypes = decltype(dog.onion.asTrait(trait::Name{}))::Types;
    static_assert(DogTypes::TypesCount == 2);
    static_assert(requires { typename DogTypes::TypesAt<0>::CatType; });
    static_assert(requires { typename DogTypes::TypesAt<1>::DogType; });

    if (42 != cat.mouse.asTrait(trait::Name{}).get())
        return false;

    if (99 != cat.onion.asTrait(trait::Name{}).get())
        return false;

    if (99 != cat.mouse.getNode(trait::Name{}).get())
        return false;

    if (42 != dog.mouse.asTrait(trait::Name{}).get())
        return false;

    if (42 != dog.mouse.getNode(trait::Name{}).get())
        return false;

    if (42 != dog.onion.asTrait(trait::Name{}).get())
        return false;

    if (not cat.mouse.asTrait(trait::Name{}).visit([]<class T>(T) { return requires { typename T::Types::MouseType; }; }))
        return false;

    if (not dog.mouse.asTrait(trait::Name{}).visit([]<class T>(T) { return requires { typename T::Types::MouseType; }; }))
        return false;

    if (not cat.onion.asTrait(trait::Name{}).visit([]<class T>(T) { return requires { typename T::Types::CatType; }; }))
        return false;

    if (not dog.onion.asTrait(trait::Name{}).visit([]<class T>(T) { return requires { typename T::Types::DogType; }; }))
        return false;

    return true;
}

} // test

DI_INSTANTIATE(di::InlineGraph<test::Union>, onion->get<1>())

int main()
{
    bool success = test::test();
    if (not success)
    {
        std::puts("test_union failed\n");
        return 1;
    }
    return 0;
}
