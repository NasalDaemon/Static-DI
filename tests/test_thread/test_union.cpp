#include <atomic>
#include <doctest/doctest.h>
#include <typeindex>
#include <utility>
#include "di/macros.hpp"

import di.tests.thread.union_;
import di.tests.thread.global_scheduler;
import di.tests.thread.poster;
import di;

/* di-embed-begin

export module di.tests.thread.union_;

namespace di::tests::thread::union_ {

trait trait::Query
{
    query() const -> int
}

trait trait::Swap
{
    swap()
}

cluster Cluster [R = Root]
{
    on0 = di::Union<typename R::Node1, typename R::Node2> : di::OnDynThread
    on1 = di::Union<typename R::Node1, typename R::Node2> : di::OnDynThread
    on2 = di::Union<di::OnDynThread<typename R::Node1>, di::OnDynThread<typename R::Node2>>
}

}

di-embed-end */

namespace di::tests::thread::union_ {

struct Node2;
struct Node1 : di::Node
{
    using Traits = di::Traits<Node1, trait::Query, trait::Swap, di::trait::DynamicNode>;

    int impl(trait::Query::query) const
    {
        CHECK(onConstructed);
        return i;
    }

    void impl(this auto& self, trait::Swap::swap)
    {
        auto defer = self.template exchangeImpl<Node2>();
        CHECK(defer.empty());
    }

    template<class Self>
    void impl(this Self& self, di::trait::DynamicNode::exchangeImpl)
    {
        CHECK(self.getGlobal(di::trait::scheduler).inExclusiveMode());
        auto defer = self.template exchangeImpl<Node2>();
        CHECK(not defer.empty());
    }

    void onGraphConstructed()
    {
        onConstructed = true;
    }

    Node1() = default;

    int const i = 1;
    bool onConstructed = false;
};
struct Node2 : di::Node
{
    using Traits = di::Traits<Node2, trait::Query, trait::Swap, di::trait::DynamicNode>;

    int impl(trait::Query::query) const
    {
        CHECK(onConstructed);
        return i;
    }

    void impl(this auto& self, trait::Swap::swap)
    {
        auto defer = self.template exchangeImpl<Node1>();
        CHECK(defer.empty());
    }

    template<class Self>
    void impl(this Self& self, di::trait::DynamicNode::exchangeImpl)
    {
        CHECK(self.getGlobal(di::trait::scheduler).inExclusiveMode());
        auto defer = self.template exchangeImpl<Node1>();
        CHECK(not defer.empty());
    }

    void onGraphConstructed()
    {
        onConstructed = true;
    }

    Node2() = default;

    int const i = 2;
    bool onConstructed = false;
};

TEST_CASE("Union")
{
    struct Root
    {
        using Node1 = union_::Node1;
        using Node2 = union_::Node2;
    };

    di::GraphWithGlobal<Cluster, GlobalScheduler, Root> g{
        .global{
            [](auto& scheduler)
            {
                scheduler.addThread();
                scheduler.addThread();
            },
        },
        .main{
            .on0{0, std::in_place_index<0>},
            .on1{1, std::in_place_index<1>},
            .on2{std::in_place_index<0>, 2},
        },
    };
    g.onConstructed();

    CHECK_THROWS_WITH(g->on0.get()->emplace<Node2>(), "di::Union::emplace can only be called when the scheduler is in exclusive mode");

    auto scheduler = g.global.asTrait(di::trait::scheduler);

    std::atomic_uint64_t taskCount = 0;

    scheduler.postTask(0, [&] {
        CHECK(1 == g->on0->asTrait(trait::query).query());
        CHECK(2 == g->on1->asTrait(trait::query, future).query().get());
        CHECK(1 == g->on2.asTrait(trait::query, future).query().get());
        taskCount++;

        scheduler.postTask(1, [&] {
            CHECK(1 == g->on0->asTrait(trait::query, future).query().get());
            CHECK(2 == g->on1->asTrait(trait::query).query());
            CHECK(1 == g->on2.asTrait(trait::query, future).query().get());
            taskCount++;

            scheduler.postTask(2, [&] {
                CHECK(1 == g->on0->asTrait(trait::query, future).query().get());
                CHECK(2 == g->on1->asTrait(trait::query, future).query().get());
                CHECK(1 == g->on2.asTrait(trait::query, future).query().get());

                g->on0.get().asTrait(trait::swap, future).swap().get();
                taskCount++;

                scheduler.postExclusiveTask(g,
                    [](auto& g)
                    {
                        CHECK(2 == g->on0->asTrait(trait::query, future).query().get());
                        g->on0.get()->template emplace<Node1>();
                        CHECK(1 == g->on0->asTrait(trait::query).query());
                        g.global.asTrait(di::trait::scheduler).stop();
                    });
            });
        });
    });

    scheduler.run();
    CHECK(taskCount.load() == 3);
}

}
