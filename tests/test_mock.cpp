module;
#include <doctest/doctest.h>
#include "di/macros.hpp"
#if !DI_STD_MODULE
#include <typeinfo>
#include <vector>
#endif
module di.test.mock;

import di;
DI_IMPORT_STD;

/* di-embed-begin

export module di.test.mock;

trait di::test::trait::MockTest
{
    takesNothing() const
    takesInt(int i)
    returnsRef() -> int&
}

di-embed-end */

using namespace di::test;

struct MockTestNode : di::Node
{
    using Traits = di::Traits<MockTestNode>;

    int testNothing(this auto& self)
    {
        return self.getNode(trait::mockTest).takesNothing();
    }
    int testInt(this auto& self, int i)
    {
        return self.getNode(trait::mockTest).takesInt(i);
    }
    int& testRef(this auto& self)
    {
        return self.getNode(trait::mockTest).returnsRef();
    }
};

TEST_CASE("di::test::Mock")
{
    di::test::Graph<MockTestNode> g;
    int i = 101;

    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::takesNothing{}));
    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::takesInt{}));

    g.mocks->setThrowIfMissing();

    g.mocks->define(
        [](trait::MockTest::takesNothing)
        {
           return 99;
        },
        [](trait::MockTest::takesInt, int i)
        {
            return 99 - i;
        },
        [&](trait::MockTest::returnsRef) -> int&
        {
            return i;
        });
    CHECK(99 == g.node->testNothing());
    CHECK(91 == g.node->testInt(8));

    int& ref = g.node->testRef();
    CHECK(101 == i);
    CHECK(101 == ref);
    i = 88;
    CHECK(88 == i);
    CHECK(88 == ref);

    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::takesNothing{}));
    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::takesInt{}));
    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::returnsRef{}));

    g.mocks->reset();

    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::takesNothing{}));
    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::takesInt{}));
    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::returnsRef{}));

    // Default behaviour is to return default value

    CHECK(0 == g.node->testNothing());
    CHECK(0 == g.node->testInt(8));

    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::takesNothing{}));
    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::takesInt{}));

    g.mocks->setThrowIfMissing();

#if DI_COMPILER_GCC
    CHECK_THROWS_WITH(g.node->testNothing(), "Mock implementation not defined for apply(di::test::trait::MockTest@di.test.mock::takesNothing) const");
    CHECK_THROWS_WITH(g.node->testInt(8), "Mock implementation not defined for apply(di::test::trait::MockTest@di.test.mock::takesInt, int)");
#else
    CHECK_THROWS_WITH(g.node->testNothing(), "Mock implementation not defined for apply(di::test::trait::MockTest::takesNothing) const");
    CHECK_THROWS_WITH(g.node->testInt(8), "Mock implementation not defined for apply(di::test::trait::MockTest::takesInt, int)");
#endif
}
