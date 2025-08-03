#ifndef INCLUDE_DI_THREAD_HPP
#define INCLUDE_DI_THREAD_HPP

#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"

#include "di/context_fwd.hpp"
#include "di/cluster.hpp"
#include "di/detached.hpp"
#include "di/environment.hpp"
#include "di/key.hpp"
#include "di/macros.hpp"
#include "di/map_info.hpp"
#include "di/node_fwd.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
#if DI_COMPILER_LT(GCC, 15)
#include <string>
#else
#include <format>
#endif
#include <stdexcept>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
struct ThreadEnvironment
{
    static constexpr std::size_t AnyThreadId = std::size_t(-1);
    static constexpr std::size_t DynamicThreadId = std::size_t(-2);

    struct Tag{};

    template<std::size_t Id>
    struct WithId
    {
        using Tag = ThreadEnvironment::Tag;
        static WithId resolveEnvironment(Tag);

        static constexpr std::size_t ThreadId = Id;
    };
};

DI_MODULE_EXPORT
struct Thread
{
    static constexpr std::size_t Unassigned = -1;

    static bool hasMainThread() { return MainThread != nullptr; }
    static bool isMainThread() { return CurrentId == 0 and MainThread == &CurrentId; }

    static void setMainThread()
    {
        if (hasMainThread())
            throw std::runtime_error("A thread has already been assigned as the main");
        CurrentId = 0;
        MainThread = &CurrentId;
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
        if (threadId == 0)
            return setMainThread();
        if (threadId == ThreadEnvironment::DynamicThreadId)
            #if DI_COMPILER_LT(GCC, 15)
                throw std::runtime_error("Thread cannot be set to id " + std::to_string(threadId));
            #else
                throw std::runtime_error(std::format("Thread cannot be set to id {}", threadId));
            #endif
        if (CurrentId != Unassigned)
            #if DI_COMPILER_LT(GCC, 15)
                throw std::runtime_error(
                    "Thread has already been assigned ID " + std::to_string(CurrentId) + " while trying to assign ID " + std::to_string(threadId));
            #else
                throw std::runtime_error(
                    std::format("Thread has already been assigned ID {} while trying to assign ID {}", CurrentId, threadId));
            #endif
        CurrentId = threadId;
    }

private:
    inline static thread_local std::size_t CurrentId = Unassigned;
    inline static void* MainThread = nullptr;
};

namespace detail {
    template<class Source>
    constexpr std::size_t getSourceStaticThread()
    {
        if constexpr (Source::Environment::template HasTag<ThreadEnvironment::Tag>)
            return Source::Environment::template Get<ThreadEnvironment::Tag>::ThreadId;

        // If source is a trait (i.e. not the node's or cluster's state), then we can assume
        // access has been already granted, and the required thread is the current thread
        if constexpr (requires { ContextOf<Source>::Info::RequiredThreadId; })
            if constexpr (not IsCluster<Source> and not IsNodeState<Source>)
                return ContextOf<Source>::Info::RequiredThreadId;

        return ThreadEnvironment::AnyThreadId;
    }

    template<class Target>
    constexpr std::size_t getTargetDynamicRequiredThread(Target& target)
    {
        if constexpr (requires { ContextOf<Target>::Info::RequiredThreadId; })
        {
            constexpr auto requiredThreadId = ContextOf<Target>::Info::RequiredThreadId;
            if constexpr (requiredThreadId != ThreadEnvironment::DynamicThreadId)
                return requiredThreadId;
        }

        using ThreadContext = ContextOf<Target>::Info::ThreadContext;
        auto parentMemPtr = ContextOf<Target>{}.template getParentMemPtr<ThreadContext>();
        using TargetNode = Target::Traits::Node;
        return getParent(upCast<TargetNode>(target), parentMemPtr).threadId;
    }

    template<class Source>
    constexpr std::size_t getSourceDynamicThread()
    {
        constexpr auto staticThreadId = getSourceStaticThread<Source>();
        if constexpr (staticThreadId != ThreadEnvironment::DynamicThreadId)
            return staticThreadId;

        return Thread::getId();
    }
}

template<std::size_t ThreadId>
struct RequireThread
{
    static_assert(ThreadId != ThreadEnvironment::DynamicThreadId);

    static consteval bool anyThreadId() { return ThreadId == ThreadEnvironment::AnyThreadId; }

    template<class Info>
    struct MapInfo : Info
    {
        static constexpr std::size_t RequiredThreadId = ThreadId;

        template<class Target>
        static constexpr void assertAccessible(Target&)
        {
            if constexpr (not anyThreadId())
            {
                using Environment = Target::Environment;
                static_assert(Environment::template HasTag<ThreadEnvironment::Tag>, "Access denied from unknown thread");
                constexpr auto threadId = Environment::template Get<ThreadEnvironment::Tag>::ThreadId;
                static_assert(threadId == RequiredThreadId, "Access denied from current thread");
            }
        }

        template<class Source, class Target, class Key = ContextOf<Source>::Info::DefaultKey>
        static constexpr auto finalize(Source& source, Target& target, Key const& key = {}, auto const&... keys)
        {
            constexpr auto currentThreadId = detail::getSourceStaticThread<Source>();
            if constexpr (anyThreadId() or currentThreadId == RequiredThreadId)
            {
                using Env = Source::Environment::template InsertOrReplace<ThreadEnvironment::WithId<currentThreadId>>;
                using WithEnv = di::WithEnv<Env, Target>;
                using Interface = Key::template Interface<WithEnv, currentThreadId>;
                return makeAlias(detail::downCast<Interface>(target), keys...);
            }
            else
            {
                return makeAlias(key.acquireAccess(source, target), keys...);
            }
        }
    };
};

// TODO: construction/destruction on the same thread
DI_MODULE_EXPORT
template<IsNodeHandle Node, std::size_t Thread>
using OnThread = MapInfo<Node, RequireThread<Thread>>;

DI_MODULE_EXPORT
template<IsNodeHandle Node>
using AnyThread = MapInfo<Node, RequireThread<ThreadEnvironment::AnyThreadId>>;

namespace key {
    DI_MODULE_EXPORT
    struct DynThreadAssert : Default
    {
        template<class Source, class Target>
        static constexpr Target& acquireAccess(Source&, Target& target)
        {
            ContextOf<Target>::Info::assertAccessible(target);
            return target;
        }
    } inline constexpr dynThreadAssert{};
}

DI_MODULE_EXPORT
template<IsNodeHandle NodeHandle>
struct OnDynThread
{
    template<class Context>
    struct Node : Cluster
    {
        static constexpr bool isUnary() { return decltype(node)::isUnary(); }

        struct Inner : di::Context<Node, NodeHandle>
        {
            static constexpr std::size_t Depth = Context::Depth;

            template<class T>
            requires detail::HasLink<Context, T>
            static auto resolveLink(T) -> ResolvedLink<Context, T>;

            struct Info : Context::Info
            {
                using DefaultKey = key::DynThreadAssert;

                using ThreadContext = Inner;
                static constexpr std::size_t RequiredThreadId = ThreadEnvironment::DynamicThreadId;

                template<class Target>
                static constexpr void assertAccessible(Target& target)
                {
                    auto requiredThreadId = detail::getTargetDynamicRequiredThread(target);
                    if (requiredThreadId == ThreadEnvironment::AnyThreadId)
                        return;

                    auto currentThreadId = Thread::getId();
                    if (currentThreadId != requiredThreadId) [[unlikely]]
                    #if DI_COMPILER_LT(GCC, 15)
                        throw std::runtime_error("Access denied to node with thread affinity " + std::to_string(requiredThreadId) + " from current thread " + std::to_string(currentThreadId));
                    #else
                        throw std::runtime_error(std::format("Access denied to node with thread affinity {} from current thread {}", requiredThreadId, currentThreadId));
                    #endif
                }

                template<class Source, class Target, class Key = ContextOf<Source>::Info::DefaultKey>
                static constexpr auto finalize(Source& source, Target& target, Key const& key = {}, auto const&... keys)
                {
                    return makeAlias(key.acquireAccess(source, target), keys...);
                }
            };
        };

        DI_NODE(Inner, node);
        std::size_t threadId;

        template<class T>
        requires HasTrait<decltype(node), T>
        static auto resolveLink(T) -> ResolvedLink<Inner, T>;

        constexpr decltype(auto) operator->(this auto& self)
        {
            if constexpr (IsNode<ContextToNode<Inner>>)
                return (self.node);
            else
                return std::addressof(self.node);
        }

        constexpr void visit(this auto& self, auto&& visitor)
        {
            self.node.visit(DI_FWD(visitor));
        }
    };
};

DI_MODULE_EXPORT
template<std::size_t ThreadId>
constexpr auto& withThread(auto& t)
{
    return mergeEnv<ThreadEnvironment::WithId<ThreadId>>(t);
}

namespace key {

    DI_MODULE_EXPORT
    template<template<std::size_t, std::size_t> class Poster>
    struct ThreadPost
    {
        template<class T>
        using Trait = di::detail::DuckTrait<T>;

        template<class T, std::size_t CurrentThreadId>
        struct Interface : T
        {
            constexpr decltype(auto) visit(this auto& self, auto&& visitor)
            {
                std::size_t dynamicRequiredThreadId = requiredThreadId;
                if constexpr (requiredThreadId == ThreadEnvironment::DynamicThreadId)
                    dynamicRequiredThreadId = detail::getTargetDynamicRequiredThread(self);
                return Poster<CurrentThreadId, requiredThreadId>::post(
                    dynamicRequiredThreadId,
                    [&, visitor = DI_FWD(visitor)](this auto&&) -> decltype(auto)
                    {
                        return self.visit(std::move(visitor));
                    });
            }

            constexpr decltype(auto) impl(this auto& self, auto&&... args)
            {
                std::size_t dynamicRequiredThreadId = requiredThreadId;
                if constexpr (requiredThreadId == ThreadEnvironment::DynamicThreadId)
                    dynamicRequiredThreadId = detail::getTargetDynamicRequiredThread(self);
                return Poster<CurrentThreadId, requiredThreadId>::post(
                    dynamicRequiredThreadId,
                    [&self, ...args = DI_FWD(args)](this auto&&) -> decltype(auto)
                    {
                        return self.T::impl(std::move(args)...);
                    });
            }

        private:
            static constexpr auto requiredThreadId = ContextOf<T>::Info::RequiredThreadId;
        };

        template<class Source, class Target>
        static constexpr auto& acquireAccess(Source&, Target& target)
        {
            constexpr auto requiredThreadId = ContextOf<Target>::Info::RequiredThreadId;
            static_assert(requiredThreadId != ThreadEnvironment::AnyThreadId);
            constexpr auto currentThreadId = detail::getSourceStaticThread<Source>();

            using Environment = Source::Environment;
            using Env = Environment::template InsertOrReplace<ThreadEnvironment::WithId<requiredThreadId>>;
            using WithEnv = di::WithEnv<Env, Target>;
            return detail::downCast<Interface<WithEnv, currentThreadId>>(target);
        }
    };

} // namespace key

} // namespace di


#endif // INCLUDE_DI_THREAD_HPP
