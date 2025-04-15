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
}

di-embed-end */

using namespace di::test;

struct MockTestNode : di::Node
{
    int testNothing(this auto& self)
    {
        return self.getNode(trait::mockTest).takesNothing();
    }
    int testInt(this auto& self, int i)
    {
        return self.getNode(trait::mockTest).takesInt(i);
    }

    using Traits = di::Traits<MockTestNode>;
};

TEST_CASE("di::test::Mock")
{
    di::test::Graph<MockTestNode> g;

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
        });
    CHECK(99 == g.node->testNothing());
    CHECK(91 == g.node->testInt(8));

    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::takesNothing{}));
    CHECK(1 == g.mocks->methodCallCount(trait::MockTest::takesInt{}));

    g.mocks->reset();

    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::takesNothing{}));
    CHECK(0 == g.mocks->methodCallCount(trait::MockTest::takesInt{}));

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
