#ifndef INCLUDE_DI_GLOBAL_TRAIT_HPP
#define INCLUDE_DI_GLOBAL_TRAIT_HPP

#include "di/macros.hpp"
#include "di/trait.hpp"

namespace di {

DI_MODULE_EXPORT
template<IsTrait T>
struct Global : T
{
    using Trait = T;

    static Global expects();
    static void canProvide(Trait);
};

namespace detail {
    template<class T>
    inline constexpr bool isGlobalTrait = false;

    template<class T>
    constexpr bool isGlobalTrait<Global<T>> = true;

    template<class T>
    concept IsGlobalTrait = isGlobalTrait<T>;
}

DI_MODULE_EXPORT
template<class T>
concept IsGlobalTrait = IsTrait<T> and detail::IsGlobalTrait<T>;

DI_MODULE_EXPORT
template<class T>
concept IsNonGlobalTrait = IsTrait<T> and not detail::IsGlobalTrait<T>;

DI_MODULE_EXPORT
template<IsGlobalTrait Trait>
constexpr Trait global(Trait = {}) { return {}; }

DI_MODULE_EXPORT
template<IsTrait Trait>
constexpr Global<Trait> global(Trait = {}) { return {}; }

} // namespace di


#endif // INCLUDE_DI_GLOBAL_TRAIT_HPP
