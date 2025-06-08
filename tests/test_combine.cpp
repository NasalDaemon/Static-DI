#include <doctest/doctest.h>
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <typeinfo>
#include <vector>
#endif

import di;

namespace di::tests::combine {

namespace trait {

    struct A : di::UnconstrainedTrait
    {
        #define DI_METHODS_A(TAG) \
            TAG(a) \
            TAG(c)
        DI_METHODS(A)
    } inline constexpr a{};

    struct B : di::UnconstrainedTrait
    {
        #define DI_METHODS_B(TAG) \
            TAG(b) \
            TAG(c)
        DI_METHODS(B)
    } inline constexpr b{};

    struct C : di::UnconstrainedTrait
    {
        #define DI_METHODS_C(TAG) \
            TAG(c)
        DI_METHODS(C)
    } inline constexpr c{};

} // namespace trait

struct A : di::Node
{
    using Traits = di::Traits<A, trait::A>;

    int apply(trait::A::a) { return val; }

    int apply(this auto& self, trait::A::c)
    {
        return self.getNode(trait::c).c();
    }

    int val = 42;
};

struct B : di::Node
{
    using Traits = di::Traits<B, trait::B>;

    int apply(trait::B::b) { return val; }

    int apply(this auto& self, trait::B::c)
    {
        return self.getNode(trait::c).c();
    }

    int val = 314;
};

struct C : di::Node
{
    using Traits = di::Traits<C, trait::C>;

    int apply(trait::C::c) { return val; }

    int val = 99;
};

TEST_CASE("di::Combine test doubles")
{
    test::Graph<C, Combine<A, B>> g;
    static_assert(sizeof(g) == 3 * sizeof(int));

    CHECK(42 == g.node.getNode(trait::a).a());
    CHECK(314 == g.node.getNode(trait::b).b());

    CHECK(99 == g.node.getNode(trait::a).c());
    CHECK(99 == g.node.getNode(trait::b).c());
}

TEST_CASE_TEMPLATE("di::Combine with Mock", Mock, test::Mock<EmptyTypes, trait::B>, Narrow<test::Mock<>, trait::B>)
{
    test::Graph<C, Combine<A, Mock>> g;

    g.mocks.template get<Mock>()->define(
        [](trait::B::b)
        {
            return 22;
        },
        [&](trait::B::c)
        {
            return g.node.asTrait(trait::c).c();
        });

    CHECK(42 == g.node.getNode(trait::a).a());
    CHECK(22 == int(g.node.getNode(trait::b).b()));

    CHECK(99 == g.node.getNode(trait::a).c());
    CHECK(99 == int(g.node.getNode(trait::b).c()));
}

} // namespace di::tests::combine
