#ifndef INCLUDE_DI_KEY_HPP
#define INCLUDE_DI_KEY_HPP

#include "di/detail/concepts.hpp"

#include "di/macros.hpp"
#include "di/trait.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#endif

namespace di {
    DI_MODULE_EXPORT
    struct Node;
    DI_MODULE_EXPORT
    struct Cluster;
    namespace detail {
        struct ContextBase;
    }
}

namespace di::key {

DI_MODULE_EXPORT
template<class T>
concept IsKey = requires {
    typename di::detail::TakesUnaryClassTemplate<T::template Trait>;
    typename di::detail::TakesUnaryClassPackedAutoTemplate<T::template Interface>;
    requires std::is_trivially_copyable_v<T>;
};

DI_MODULE_EXPORT
template<class Key, IsTrait Trait_>
using Trait = Key::template Trait<Trait_>;

DI_MODULE_EXPORT
struct Default
{
    template<class T>
    using Trait = T;
    template<class T, auto... Info>
    using Interface = T;

    template<class Environment, auto SourceInfo, class Target>
    static constexpr Target& acquireAccess(Target& target)
    {
        static_assert(di::detail::alwaysFalse<Environment>, "Access denied");
        return target;
    }
};

// Bypass TraitView instantiation during intermediate asTrait/getNode calls
struct Bypass : private Default
{
    using Default::Trait;
    using Default::Interface;
    using Default::acquireAccess;

private:
    friend struct di::Node;
    friend struct di::Cluster;
    friend struct di::detail::ContextBase;
    Bypass() = default;
};


}

#endif // INCLUDE_DI_KEY_HPP
