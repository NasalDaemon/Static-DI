#ifndef INCLUDE_DI_THREAD_HPP
#define INCLUDE_DI_THREAD_HPP

#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"

#include "di/context_fwd.hpp"
#include "di/detached.hpp"
#include "di/environment.hpp"
#include "di/macros.hpp"
#include "di/map_info.hpp"
#include "di/node_fwd.hpp"
#include "di/trait.hpp"

#if !DI_STD_MODULE
#include <cstddef>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
struct ThreadEnvironment
{
    static constexpr std::size_t AnyThreadId = std::size_t(-1);

    struct Tag{};

    template<std::size_t Id>
    struct WithId
    {
        using Tag = ThreadEnvironment::Tag;
        static WithId resolveEnvironment(Tag);

        static constexpr std::size_t ThreadId = Id;
    };
};

namespace detail {
    template<class Source>
    consteval std::size_t getCurrentThread()
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
}

template<std::size_t ThreadId>
struct RequireThread
{
    static consteval bool anyThreadId() { return ThreadId == ThreadEnvironment::AnyThreadId; }

    template<class Info>
    struct MapInfo : Info
    {
        static constexpr auto RequiredThreadId = ThreadId;

        template<class Environment>
        static consteval void assertAccessible()
        {
            if constexpr (not anyThreadId())
            {
                static_assert(Environment::template HasTag<ThreadEnvironment::Tag>, "Access denied from unknown thread");
                static_assert(Environment::template Get<ThreadEnvironment::Tag>::ThreadId == RequiredThreadId, "Access denied from current thread");
            }
        }

        template<class Source, class Target, class Key>
        static constexpr auto finalize(Source&, Target& target, Key)
        {
            constexpr auto currentThreadId = detail::getCurrentThread<Source>();
            if constexpr (anyThreadId() or currentThreadId == RequiredThreadId)
            {
                using Env = Source::Environment::template InsertOrReplace<ThreadEnvironment::WithId<currentThreadId>>;
                using WithEnv = di::WithEnv<Env, Target>;
                using Interface = Key::template Interface<WithEnv, currentThreadId>;
                return makeAlias(detail::downCast<Interface>(target));
            }
            else
            {
                return makeAlias(Key::template acquireAccess<
                    typename Source::Environment,
                    RequiredThreadId
                >(target));
            }
        }
    };
};

// TODO: Proper construction/destruction/visit
DI_MODULE_EXPORT
template<IsNodeHandle Node, std::size_t Thread>
using OnThread = MapInfo<Node, RequireThread<Thread>>;

DI_MODULE_EXPORT
template<IsNodeHandle Node>
using AnyThread = MapInfo<Node, RequireThread<ThreadEnvironment::AnyThreadId>>;

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
            constexpr decltype(auto) visit(this auto& self, auto const& f)
            {
                return Poster<CurrentThreadId, requiredThreadId>::post([&]() -> decltype(auto) { return self.visit(f); });
            }

            constexpr decltype(auto) apply(this auto& self, auto&&... args)
            {
                return Poster<CurrentThreadId, requiredThreadId>::post(
                    [&]() -> decltype(auto)
                    {
                        return self.T::apply(DI_FWD(args)...);
                    });
            }

        private:
            static constexpr auto requiredThreadId = ContextOf<T>::Info::RequiredThreadId;
        };

        template<class Environment, std::size_t CurrentThreadId, class Target>
        static constexpr auto& acquireAccess(Target& target)
        {
            constexpr auto requiredThreadId = ContextOf<Target>::Info::RequiredThreadId;
            static_assert(requiredThreadId != ThreadEnvironment::AnyThreadId);
            using Env = Environment::template InsertOrReplace<ThreadEnvironment::WithId<requiredThreadId>>;
            using WithEnv = di::WithEnv<Env, Target>;
            return detail::downCast<Interface<WithEnv, CurrentThreadId>>(target);
        }
    };

} // namespace key

} // namespace di


#endif // INCLUDE_DI_THREAD_HPP
