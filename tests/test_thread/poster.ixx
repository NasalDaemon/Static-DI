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

    template<class>
    struct Function;

    template<class R, class... Args>
    struct Function<R(Args...)>
    {
        Function() = default;

        template<std::invocable<Args...> F>
        Function(F&& f)
            : callable{makeCallable(DI_FWD(f))}
        {}

        R operator()(auto&&... args) const
        {
            return callable->fptr(callable.get(), DI_FWD(args)...);
        }

        operator bool() const { return callable; }

    private:
        struct CallableBase
        {
            using Sig = R(*)(CallableBase*, Args...);
            Sig fptr;
            using Dtor = void(*)(CallableBase*);
            Dtor dptr;
        };
        template<class F>
        struct Callable : CallableBase
        {
            [[no_unique_address]] F f;
        };
        struct Deleter
        {
            constexpr void operator()(CallableBase* p) const { p->dptr(p); }
        };

        template<class F>
        static CallableBase* makeCallable(F&& f)
        {
            using T = Callable<std::remove_cvref_t<F>>;
            return new T{
                {
                    .fptr{
                        [](CallableBase* base, Args... args) -> R
                        {
                            return std::invoke_r<R>(static_cast<T*>(base)->f, DI_FWD(args)...);
                        }
                    },
                    .dptr{
                        [](CallableBase* base)
                        {
                            delete static_cast<T*>(base);
                        }
                    },
                },
                DI_FWD(f),
            };
        }

        std::unique_ptr<CallableBase, Deleter> callable;
    };

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
            stopAll();
            run();
            Thread::resetMainThread();
        }
    };

    export [[nodiscard]] bool postTask(std::weak_ptr<Scheduler::ThreadContext> h, Function<void()> f);

    template<std::size_t CurrentThreadId, std::size_t RequiredThreadId>
    struct FireAndForget
    {
        template<class Task>
        static constexpr bool post(std::size_t requiredThreadId, Task task)
        {
            if constexpr (CurrentThreadId == ThreadEnvironment::DynamicThreadId
                       or RequiredThreadId == ThreadEnvironment::DynamicThreadId
                       or CurrentThreadId != RequiredThreadId)
            {
                if (auto ts = Scheduler::get().lock()) [[likely]]
                    if (ts->postTask(requiredThreadId, std::move(task))) [[likely]]
                        return true;
                return false;
            }
            else
            {
                // Already statically known to be on correct thread
                task();
                return true;
            }
        }
    };

    export inline constexpr di::key::ThreadPost<FireAndForget> fireAndForget{};

    template<std::size_t CurrentThreadId, std::size_t RequiredThreadId>
    struct Future
    {
        template<std::invocable Task>
        static constexpr std::future<std::invoke_result_t<Task>> post(std::size_t requiredThreadId, Task&& task)
        {
            using R = std::invoke_result_t<Task>;

            if constexpr (CurrentThreadId == ThreadEnvironment::DynamicThreadId
                       or RequiredThreadId == ThreadEnvironment::DynamicThreadId
                       or CurrentThreadId != RequiredThreadId)
            {
                if (auto ts = Scheduler::get().lock()) [[likely]]
                {
                    auto package = std::packaged_task<R()>(DI_FWD(task));
                    auto future = package.get_future();
                    if (ts->postTask(requiredThreadId, std::move(package))) [[likely]]
                        return future;
                }
                if (Thread::getId() != requiredThreadId)
                {
                    std::promise<R> promise;
                    promise.set_exception(std::make_exception_ptr(
                        std::runtime_error(std::format("Unable to post task to thread {}", requiredThreadId))));
                    return promise.get_future();
                }
            }
            // Already known to be on correct thread
            std::promise<R> promise;
            try
            {
                promise.set_value(std::invoke(task));
            }
            catch (...)
            {
                promise.set_exception(std::current_exception());
            }
            return promise.get_future();
        }
    };

    export inline constexpr di::key::ThreadPost<Future> future{};
}
