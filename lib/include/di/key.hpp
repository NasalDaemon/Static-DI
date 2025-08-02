#ifndef INCLUDE_DI_KEY_HPP
#define INCLUDE_DI_KEY_HPP

#include "di/detail/concepts.hpp"

#include "di/macros.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di::key {

DI_MODULE_EXPORT
template<class T>
concept IsKey = requires {
    typename di::detail::TakesUnaryClassTemplate<T::template Trait>;
    typename di::detail::TakesUnaryClassPackedAutoTemplate<T::template Interface>;
    requires std::is_trivially_copyable_v<T>;
};

DI_MODULE_EXPORT
template<IsKey Key, IsTrait Trait_>
using Trait = Key::template Trait<Trait_>;

DI_MODULE_EXPORT
struct Default
{
    template<class T>
    using Trait = T;
    template<class T, auto... Info>
    using Interface = T;

    template<class Source, class Target>
    static constexpr Target& acquireAccess(Source&, Target& target)
    {
        static_assert(di::detail::alwaysFalse<Source>, "Access denied");
        return target;
    }
};

}

#endif // INCLUDE_DI_KEY_HPP
