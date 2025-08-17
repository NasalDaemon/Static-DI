#ifndef INCLUDE_DI_FINALIZE_HPP
#define INCLUDE_DI_FINALIZE_HPP

#include "di/alias.hpp"
#include "di/context_fwd.hpp"
#include "di/detail/concepts.hpp"
#include "di/environment.hpp"
#include "di/macros.hpp"
#include "di/key.hpp"

#if !DI_IMPORT_STD
#include <tuple>
#endif

namespace di {

namespace detail {

    template<class Source, class Target>
    consteval auto accessTagsRequiredFromKeys()
    {
        auto sourceRequires = ContextOf<Source>::Info::template requiresKeysToTarget<Source, Target>();
        auto targetRequires = ContextOf<Target>::Info::template requiresKeysToTarget<Source, Target>();
        return std::tuple_cat(sourceRequires, targetRequires);
    }

    template<class Source, class Target, di::key::IsKey Key>
    consteval bool shouldAcquireAccess()
    {
        auto accessTags = accessTagsRequiredFromKeys<Source, Target>();
        if (std::tuple_size_v<decltype(accessTags)> > 0)
        {
            std::apply(
                []<class... Tags>(Tags...)
                {
                    Key::template assertCanAcquireAccess<Tags...>();
                },
                accessTags);
            return true;
        }
        return false;
    }

} // namespace detail

DI_MODULE_EXPORT
template<class Source, class Target>
DI_INLINE constexpr auto finalize(Source&, Target& target)
{
    constexpr auto accessTags = detail::accessTagsRequiredFromKeys<Source, Target>();
    if constexpr (std::tuple_size_v<decltype(accessTags)> != 0)
        static_assert(detail::alwaysFalse<Source, Target>, "Incorrect or insufficient keys to acquire access to target from source");
    return makeAlias(target);
}

// If ConsumeKey is false, the first key not needed to acquire access will be included in the alias
DI_MODULE_EXPORT
template<bool ConsumeKey = true, class Source, class Target, class Key, class... Keys>
constexpr auto finalize(Source& source, Target& target, Key const& key, Keys const&... keys)
{
    if constexpr (detail::shouldAcquireAccess<Source, Target, Key>())
    {
        // Allow consuming the key to acquire access as we will not know original source after making the alias
        auto& target2 = key.acquireAccess(source, target);
        return finalize(source, target2, keys...);
    }
    else
    {
        using Env = Source::Environment;
        using WithEnv = di::WithEnv<Env, Target>;
        if constexpr (ConsumeKey)
        {
            using FinalInterface = Key::template Interface<WithEnv>;
            return makeAlias(detail::downCast<FinalInterface>(target), keys...);
        }
        else
        {
            return makeAlias(detail::downCast<WithEnv>(target), key, keys...);
        }
    }
}

} // namespace di

#endif // INCLUDE_DI_FINALIZE_HPP
