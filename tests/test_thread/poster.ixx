module;
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <atomic>
#include <concepts>
#include <cstddef>
#include <exception>
#include <format>
#include <functional>
#include <future>
#include <latch>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#endif
export module di.tests.thread.poster;

import di;

namespace di::tests::thread {

    export struct Scheduler
    {
        struct ThreadContext;

    private:
        inline static std::weak_ptr<Scheduler> s_current{};
        std::shared_ptr<ThreadContext> mainThreadContext{};
        std::vector<std::weak_ptr<ThreadContext>> threadContexts{};
        std::vector<std::jthread> threads{};
        std::mutex threadsMtx{};
        std::atomic_bool sealed = false;
        std::atomic<std::latch*> stop_latch{nullptr};

        struct Private{};

    public:
        Scheduler(Private);
        Scheduler(Scheduler const&) = delete;
        Scheduler(Scheduler&&) = delete;

        static std::shared_ptr<Scheduler> make();
        static std::weak_ptr<Scheduler> get() { return s_current; }

        std::weak_ptr<ThreadContext> addThread();
        std::weak_ptr<ThreadContext> getThread(std::size_t id);
        void startThreads();

        void run();

        [[nodiscard]] bool postTask(std::size_t threadId, Function<void()> task);

        void stopAll();

        ~Scheduler()
        {
            auto g = di::Defer([] { Thread::resetMainThread(); });
            stopAll();
            run();
        }
    };

    export [[nodiscard]] bool postTask(std::weak_ptr<Scheduler::ThreadContext> h, Function<void()> f);

    struct FireAndForget : di::key::ThreadPost<FireAndForget>
    {
        template<std::size_t CurrentThreadId, std::size_t RequiredThreadId, class Task>
        [[nodiscard]] static constexpr bool post(std::size_t requiredThreadId, Task task)
        {
            if constexpr (CurrentThreadId == ThreadEnvironment::DynamicThreadId
                       or RequiredThreadId == ThreadEnvironment::DynamicThreadId
                       or CurrentThreadId != RequiredThreadId)
            {
                if (Thread::getId() != requiredThreadId)
                {
                    if (auto ts = Scheduler::get().lock()) [[likely]]
                        if (ts->postTask(requiredThreadId, std::move(task))) [[likely]]
                            return true;
                    return false;
                }
            }
            // Already known to be on correct thread
            task();
            return true;
        }
    };

    export inline constexpr FireAndForget fireAndForget{};

    struct Future : di::key::ThreadPost<Future>
    {
        template<std::size_t CurrentThreadId, std::size_t RequiredThreadId, std::invocable Task>
        [[nodiscard]] static constexpr auto post(std::size_t requiredThreadId, Task&& task)
            -> std::future<std::invoke_result_t<std::remove_cvref_t<Task>>>
        {
            using R = std::invoke_result_t<std::remove_cvref_t<Task>>;
            auto package = std::packaged_task<R()>(DI_FWD(task));
            auto future = package.get_future();

            if constexpr (CurrentThreadId == ThreadEnvironment::DynamicThreadId
                       or RequiredThreadId == ThreadEnvironment::DynamicThreadId
                       or CurrentThreadId != RequiredThreadId)
            {
                if (Thread::getId() != requiredThreadId)
                {
                    if (auto ts = Scheduler::get().lock()) [[likely]]
                        if (ts->postTask(requiredThreadId, std::move(package))) [[likely]]
                            return future;

                    std::promise<R> promise;
                    promise.set_exception(std::make_exception_ptr(
                        std::runtime_error(std::format("Unable to post task to thread {}", requiredThreadId))));
                    future = promise.get_future();
                    return future;
                }
            }
            // Already known to be on correct thread
            package();
            return future;
        }
    };

    export inline constexpr Future future{};
}
