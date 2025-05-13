#ifndef INCLUDE_DI_ENVIRONMENT_HPP
#define INCLUDE_DI_ENVIRONMENT_HPP

#include "di/detail/cast.hpp"
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
template<class E, class Tag>
struct EnvironmentOverlay : Tag
{
    using Environment = E;
};

DI_MODULE_EXPORT
template<class T>
concept EnvironmentComponent = requires
{
    typename T::Tag;
    requires std::is_empty_v<typename T::Tag>;
    { T::resolveEnvironment(typename T::Tag{}) } -> std::same_as<T>;
    requires std::is_empty_v<T>;
};

DI_MODULE_EXPORT
template<EnvironmentComponent... Es>
struct Environment : Es...
{
    static void resolveEnvironment();
    using Es::resolveEnvironment...;

    static constexpr bool IsEmpty = sizeof...(Es) == 0;

    template<class Tag>
    static constexpr bool HasTag = requires (Tag tag) { resolveEnvironment(tag); };

    template<class Tag>
    using Get = decltype(resolveEnvironment(Tag{}));

    template<class ToReplace, std::same_as<ToReplace> Old, class Replacement>
    static auto replaceEach(Old, Replacement) -> Replacement;

    template<class ToReplace, class Old, class Replacement>
    static auto replaceEach(Old, Replacement) -> Old;

    template<EnvironmentComponent E>
    using Replace = Environment<decltype(replaceEach<Get<typename E::Tag>>(Es{}, E{}))...>;

    template<class E>
    requires HasTag<typename E::Tag>
    static auto insertOrReplace() -> Replace<E>;

    template<class E>
    static auto insertOrReplace() -> Environment<E, Es...>;

    template<class E>
    using InsertOrReplace = decltype(insertOrReplace<E>());

    template<class... E2s>
    requires IsEmpty
    static auto merge() -> Environment<E2s...>;

    template<class E2, class... E2s>
    requires (not IsEmpty)
    static auto merge() -> decltype(InsertOrReplace<E2>::template merge<E2s...>());

    template<class... E2s>
    requires (sizeof...(E2s) == 0)
    static auto merge() -> Environment;

    template<class... E2s>
    using Merge = decltype(merge<E2s...>());

    template<class... E2s>
    static auto mergeEnvironment(Environment<E2s...> const&) -> decltype(merge<E2s...>());

    template<class Env>
    using MergeEnvironment = decltype(mergeEnvironment(std::declval<Env>()));
};

DI_MODULE_EXPORT
template<EnvironmentComponent... Es, class Target>
constexpr auto& mergeEnv(Target& t)
{
    static_assert(Target::Environment::IsEmpty);
    static_assert(sizeof...(Es) > 0);
    return detail::downCast<EnvironmentOverlay<Environment<Es...>, std::remove_const_t<Target>>>(t);
}

DI_MODULE_EXPORT
template<class Environment, class Target>
requires (not EnvironmentComponent<Environment>)
constexpr auto& withEnv(Target& t)
{
    static_assert(Target::Environment::IsEmpty);
    if constexpr (Environment::IsEmpty)
        return t;
    else
        return downCast<EnvironmentOverlay<Environment, std::remove_const_t<Target>>>(t);
}

namespace detail {
    template<class Env, class Target>
    auto withEnv() -> EnvironmentOverlay<Env, Target>;
    template<class Env, class Target>
    requires Env::IsEmpty
    auto withEnv() -> Target;
}

DI_MODULE_EXPORT
template<class Env, class Target>
using WithEnv = decltype(detail::withEnv<Env, std::remove_const_t<Target>>());

}


#endif // INCLUDE_DI_ENVIRONMENT_HPP
