#ifndef INCLUDE_DI_FACTORY_HPP
#define INCLUDE_DI_FACTORY_HPP

#include "di/macros.hpp"

namespace di {

DI_MODULE_EXPORT
struct WithFactory {} inline constexpr withFactory{};

DI_MODULE_EXPORT
template<class Type>
struct Constructor
{
    using type = Type;

    template<class... Args>
    requires std::constructible_from<Type, Args...>
    constexpr Type operator()(Args&&... args) const
    {
        return Type{DI_FWD(args)...};
    }
};

}

#endif // INCLUDE_DI_FACTORY_HPP
