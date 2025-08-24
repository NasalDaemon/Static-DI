module;
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <atomic>
#include <condition_variable>
#include <format>
#include <functional>
#include <future>
#include <iostream>
#include <latch>
#include <memory>
#include <mutex>
#include <print>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#endif

// #include <signal.h>
module di.tests.thread.poster;

import di;

namespace di::tests::thread {

struct Thread
{
    static constexpr std::size_t Unassigned = -1;

    static bool hasMainThread() { return MainThread != nullptr; }
    static bool isMainThread() { return CurrentId == ThreadEnvironment::MainThreadId and MainThread == getCurrentThreadIdPtr(); }

    static void setMainThread()
    {
        if (hasMainThread())
            throw std::runtime_error("A thread has already been assigned as the main");
        CurrentId = ThreadEnvironment::MainThreadId;
        MainThread = getCurrentThreadIdPtr();
    }

    static void resetMainThread()
    {
        if (not isMainThread())
            throw std::runtime_error("Resetting main thread from thread other than the main thread");
        CurrentId = Unassigned;
        MainThread = nullptr;
    }

    static std::size_t getId()
    {
        return CurrentId;
    }

    static void setId(std::size_t threadId)
    {
        if (threadId == ThreadEnvironment::MainThreadId)
            return setMainThread();
        if (threadId == ThreadEnvironment::DynamicThreadId || threadId == ThreadEnvironment::AnyThreadId)
            throw std::runtime_error(std::format("Thread cannot be set to id {}", threadId));
        if (CurrentId != Unassigned)
            throw std::runtime_error(
                std::format("Thread has already been assigned ID {} while trying to assign ID {}", CurrentId, threadId));
        CurrentId = threadId;
    }

    static void* getCurrentThreadIdPtr() { return &CurrentId; }

private:
    inline static thread_local std::size_t CurrentId = Unassigned;
    inline static void* MainThread = nullptr;
};

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

    bool isCurrentThread() const
    {
        return threadIdPtr == Thread::getCurrentThreadIdPtr();
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
    void* const threadIdPtr = Thread::getCurrentThreadIdPtr();
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

Scheduler::Scheduler()
{
    mainThreadContext = std::make_shared<ThreadContext>(0);
    threadContexts.push_back(mainThreadContext);
    // signal(SIGINT, [](int)
    // {
    //     std::puts("SIGINT detected, stopping all threads...");
    //     if (auto tc = Scheduler::get().lock())
    //         tc->stopAll();
    // });
}

void Scheduler::resetMainThread()
{
    Thread::resetMainThread();
}

bool Scheduler::isCurrentThread(std::size_t threadId) const
{
    return getThread(threadId).lock()->isCurrentThread();
}

std::string Scheduler::currentThreadDetails()
{
    return std::format("ID: {}", Thread::getId());
}

auto Scheduler::addThread(std::weak_ptr<ThreadContext> previous) -> std::weak_ptr<ThreadContext>
{
    if (sealed.load(std::memory_order_acquire))
        throw std::runtime_error("Cannot add threads after some have started");
    auto lk = std::lock_guard(threadsMtx);
    if (auto p = previous.lock())
    {
        threadContexts.emplace_back(previous);
        return previous;
    }
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

auto Scheduler::getThread(std::size_t threadId) const -> std::weak_ptr<ThreadContext>
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
