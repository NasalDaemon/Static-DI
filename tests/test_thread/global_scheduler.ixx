module;
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
#include <concepts>
#include <functional>
#include <string>
#include <typeindex>
#endif
export module di.tests.thread.global_scheduler;

import di.tests.thread.poster;
import di;

namespace di::tests::thread {

struct GlobalScheduler_ : di::Node
{
    using Traits = di::Traits<GlobalScheduler_, di::trait::Scheduler>;

    struct Types
    {
        using ThreadIdType = std::size_t;
    };

    std::string impl(di::trait::Scheduler::currentThreadDetails) const
    {
        return Scheduler::currentThreadDetails();
    }

    bool impl(di::trait::Scheduler::isCurrentThread, std::size_t threadId) const
    {
        return scheduler.isCurrentThread(threadId);
    }

    bool impl(di::trait::Scheduler::inExclusiveMode) const
    {
        return scheduler.isExclusiveMode();
    }

    template<IsRootCluster Cluster, StatelessInvocable<Cluster&> Task>
    bool impl(di::trait::Scheduler::postExclusiveTask, Cluster& cluster, Task&& task) const
    {
        return scheduler.postExclusiveTask(
            [task = DI_FWD(task), &cluster]() mutable
            {
                task(cluster);
            });
    }

    template<IsStateless Tag, IsRootCluster Cluster, StatelessInvocable<Cluster&> Task>
    bool impl(di::trait::Scheduler::postExclusiveTask, Tag, Cluster& cluster, Task&& task) const
    {
        return scheduler.postExclusiveTask(
            std::type_index{typeid(Tag)},
            [task = DI_FWD(task), &cluster]() mutable
            {
                task(cluster);
            });
    }

    bool impl(di::trait::Scheduler::postTask, std::size_t threadId, auto&& task) const
    {
        return scheduler.postTask(threadId, DI_FWD(task));
    }

    void impl(di::trait::Scheduler::run)
    {
        scheduler.run();
    }

    void impl(di::trait::Scheduler::stop)
    {
        scheduler.stopAll();
    }

    explicit GlobalScheduler_(std::invocable<Scheduler&> auto f)
    {
        f(scheduler);
        scheduler.startThreads();
    }

    Scheduler mutable scheduler;
};

export
using GlobalScheduler = di::AnyThread<GlobalScheduler_>;

}
