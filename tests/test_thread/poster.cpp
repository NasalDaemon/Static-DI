module;
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <condition_variable>
#include <csignal>
#include <cstddef>
#include <exception>
#include <format>
#include <future>
#include <latch>
#include <memory>
#include <mutex>
#include <print>
#include <queue>
#include <string>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>
#endif

#include <signal.h>
module di.tests.thread.poster;

import di;

namespace di::tests::thread {

struct Scheduler::ThreadContext
{
    explicit ThreadContext(std::size_t threadId)
        : threadId(threadId)
    {
        Thread::setId(threadId);
    }

    bool postTask(auto&& task)
    {
        {
            auto lk = std::lock_guard(mtx);
            if (stop_on_empty) [[unlikely]]
            {
                std::println("Thread {} is stopping, cannot post task", threadId);
                return false;
            }
            tasks.push(DI_FWD(task));
        }
        cv.notify_one();
        return true;
    }

    std::size_t getThreadId() const { return threadId; }

    void stop()
    {
        stop_on_empty = true;
        cv.notify_one();
    }

    void assertCurrentThread()
    {
        if (threadId != Thread::getId()) [[unlikely]]
        {
            throw std::runtime_error(std::format("Incorrect thread id {}, expected {}", Thread::getId(), threadId));
        }
    }

    ~ThreadContext()
    {
        std::println("Thread {} finished", threadId);
    }

private:
    friend Scheduler;

    void run()
    {
        while (true)
        {
            Function<void()> task;
            {
                auto lk = std::unique_lock(mtx);
                cv.wait(lk, [&] { return not tasks.empty() or stop_on_empty; });

                if (stop_on_empty and tasks.empty())
                    return;

                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    std::size_t const threadId;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<Function<void()>> tasks;
    std::atomic<std::latch*> latch;
    bool stop_on_empty = false;
};

bool postTask(std::weak_ptr<Scheduler::ThreadContext> h, Function<void()> f)
{
    if (auto ctx = h.lock()) [[likely]]
        return ctx->postTask(std::move(f));

    return false;
}

Scheduler::Scheduler(Private) {}

std::shared_ptr<Scheduler> Scheduler::make()
{
    if (auto current = s_current.lock())
        return current;
    auto main = std::make_shared<Scheduler>(Private());
    main->mainThreadContext = std::make_shared<ThreadContext>(0);
    main->threadContexts.push_back(main->mainThreadContext);
    s_current = main;
    signal(SIGINT, [](int)
    {
        std::puts("SIGINT detected, stopping all threads...");
        if (auto tc = Scheduler::get().lock())
            tc->stopAll();
    });
    return main;
}

auto Scheduler::addThread() -> std::weak_ptr<ThreadContext>
{
    if (sealed.load(std::memory_order_acquire))
        throw std::runtime_error("Cannot add threads after some have started");
    auto lk = std::lock_guard(threadsMtx);
    auto threadId = threadContexts.size();
    std::promise<std::weak_ptr<ThreadContext>> newTc;
    auto thread = std::jthread{
        [&]()
        {
            std::shared_ptr<ThreadContext> threadContext;
            try
            {
                threadContext = std::make_shared<ThreadContext>(threadId);
                threadContexts.emplace_back(threadContext);
                newTc.set_value(threadContext);
            }
            catch (...)
            {
                return newTc.set_exception(std::current_exception());
            }

            // Wait until threads have been set up before starting any work
            sealed.wait(false, std::memory_order_acquire);

            auto guard = Defer(
                [&]
                {
                    std::latch* platch = nullptr;
                    while (platch == nullptr)
                    {
                        stop_latch.wait(nullptr, std::memory_order_relaxed);
                        platch = stop_latch.load(std::memory_order_acquire);
                    }
                    platch->count_down();
                });

            try
            {
                threadContext->run();
            }
            catch(...)
            {
                threadContext.reset();
                stopAll();
                throw;
            }
        }
    };

    auto result = newTc.get_future().get();
    threads.emplace_back(std::move(thread));
    return result;
}

auto Scheduler::getThread(std::size_t threadId) -> std::weak_ptr<ThreadContext>
{
    if (not sealed.load(std::memory_order_acquire)) [[unlikely]]
        throw std::runtime_error("Cannot getThread before threads have started");
    return threadContexts.at(threadId);
}

void Scheduler::startThreads()
{
    sealed.store(true, std::memory_order_release);
    sealed.notify_all();
}

void Scheduler::stopAll()
{
    auto lk = std::lock_guard(threadsMtx);
    for (auto& tc : threadContexts)
        if (auto c = tc.lock())
            c->stop();
    startThreads();
}

bool Scheduler::postTask(std::size_t threadId, Function<void()> task)
{
    return thread::postTask(getThread(threadId), std::move(task));
}

void Scheduler::run()
{
    if (not Thread::isMainThread())
        throw std::runtime_error("run must be run from the main thread");
    std::latch latch(threads.size());
    stop_latch.store(&latch, std::memory_order_release);
    stop_latch.notify_all();
    mainThreadContext->run();
    latch.wait();
    threads.clear();
    threadContexts.clear();
}

}
