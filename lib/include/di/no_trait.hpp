#ifndef INCLUDE_DI_NULL_TRAIT_HPP
#define INCLUDE_DI_NULL_TRAIT_HPP

#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/trait.hpp"
#include "di/traits_fwd.hpp"

namespace di {

DI_MODULE_EXPORT
template<IsNodeHandle Node>
struct NoTrait : Trait
{
    template<class...>
    using Implements = void;
};

DI_MODULE_EXPORT
template<IsNodeHandle Node>
inline constexpr NoTrait<Node> noTrait{};

DI_MODULE_EXPORT
struct NullTrait : Trait
{
    template<class T>
    static void canProvide(NoTrait<T>);

    template<class...>
    using Implements = void;
};

namespace detail {
    template<class T>
    inline constexpr bool isNoTrait = false;

    template<IsNodeHandle Node>
    constexpr bool isNoTrait<NoTrait<Node>> = true;
}

DI_MODULE_EXPORT
template<class T>
concept IsNoTrait = IsTrait<T> and detail::isNoTrait<T>;

DI_MODULE_EXPORT
template<class T>
using NoTraits = di::Traits<T, NullTrait>;

}


#endif // INCLUDE_DI_NULL_TRAIT_HPP
