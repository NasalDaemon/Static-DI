#ifndef INCLUDE_DI_FACTORY_HPP
#define INCLUDE_DI_FACTORY_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#endif

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

DI_MODULE_EXPORT
template<class F>
struct Emplace
{
    template<class Type>
    explicit constexpr operator Type() const
    {
        std::same_as<Type> decltype(auto) result = factory(Constructor<Type>{});
        return result;
    }

    [[no_unique_address]] F factory;
};

template<class F>
Emplace(F) -> Emplace<F>;

}

#endif // INCLUDE_DI_FACTORY_HPP
