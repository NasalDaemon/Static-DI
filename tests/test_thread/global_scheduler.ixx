module;
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
#include <concepts>
#include <functional>
#include <string>
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

    bool impl(di::trait::Scheduler::postTask, std::size_t threadId, auto&& task) const
    {
        return scheduler.postTask(threadId, DI_FWD(task));
    }

    void impl(di::trait::Scheduler::run) const
    {
        scheduler.run();
    }

    void impl(di::trait::Scheduler::stop) const
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
