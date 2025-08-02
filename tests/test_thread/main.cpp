#include <doctest/doctest.h>

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <typeinfo>
#endif

import di.tests.thread.cluster;
import di.tests.thread.poster;
import di;

namespace di::tests::thread {

TEST_CASE("TestThread")
{
    di::Graph<Cluster> g{
        .a{1},
        .b{2},
        .c{3},
        .dynA{.node{4}, .threadId = 1},
        .dynB{.node{5}, .threadId = 2},
        .dynC{.node{6}, .threadId = 3},
    };

    auto scheduler = Scheduler::make();
    auto t1 = scheduler->addThread();
    auto t2 = scheduler->addThread();
    auto t3 = scheduler->addThread();
    scheduler->startThreads();

    CHECK(1 == di::withThread<1>(g.a)->i);

    auto task =
        [&]
        {
            g.a.asTrait(trait::a, fireAndForget).getA();
            CHECK(1 == g.a.asTrait(trait::a, future).getA().get());

            g.a.asTrait(trait::a, fireAndForget).getB();
            CHECK(2 == g.a.asTrait(trait::a, future).getB().get());

            CHECK_THROWS_WITH(g.dynA.node.asTrait(trait::a), "Access denied to node with thread affinity 1 from current thread 0");
            CHECK(5 == g.dynA.node.asTrait(trait::a, future).getB().get());

            scheduler->stopAll();
         };

    CHECK(scheduler->postTask(0, task));
    scheduler->run();
}

} // namespace di::tests::thread
