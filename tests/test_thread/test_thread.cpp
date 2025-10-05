#include <doctest/doctest.h>

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <typeinfo>
#endif

import di.tests.thread.cluster;
import di.tests.thread.global_scheduler;
import di.tests.thread.poster;
import di;

namespace di::tests::thread {

TEST_CASE("TestThread")
{
    di::GraphWithGlobal<Cluster, GlobalScheduler> g{
        .global{
            [](auto& scheduler)
            {
                auto t1 = scheduler.addThread();
                auto t2 = scheduler.addThread();
                auto t3 = scheduler.addThread();
            }
        },
        .main{
            .a{1},
            .b{2},
            .c{3},
            .dynA{1, 4},
            .dynB{2, 5},
            .dynC{3, 6},
        },
    };
    g.onConstructed();

    CHECK(1 == di::withThread<1>(g->a)->i);

    auto scheduler = g.global.asTrait(di::trait::scheduler);

    auto task =
        [&]
        {
            g->a.asTrait(trait::a, fireAndForget).getA();
            CHECK(1 == g->a.asTrait(trait::a, future).getA().get());

            g->a.asTrait(trait::a, fireAndForget).getB();
            CHECK(2 == g->a.asTrait(trait::a, future).getB().get());

            // Getting a detached node on the wrong thread should now throw
            auto aOnThread0 = g->dynA->asTrait(trait::a);
            // Accessing the state will throw an exception
            CHECK_THROWS_WITH(aOnThread0.getA(), "Access denied to node with thread affinity 1 from current thread: ID: 0");
            CHECK(5 == g->dynA->asTrait(trait::a, future).getB().get());

            scheduler.stop();
         };

    CHECK(scheduler.postTask(0, task));
    scheduler.run();
}

} // namespace di::tests::thread
