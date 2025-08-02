#ifndef INCLUDE_DI_DEFER_HPP
#define INCLUDE_DI_DEFER_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <optional>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
template<std::invocable F>
struct [[nodiscard, maybe_unused]] Defer
{
    Defer(auto&&... args)
        : onExit(std::in_place, DI_FWD(args)...)
    {}
#if DI_COMPILER_LT(GCC, 15)
    Defer(F&& f)
        : onExit(std::in_place, std::move(f))
    {}
#endif
    Defer(Defer&& other)
        : onExit(std::exchange(other.onExit, std::nullopt))
    {}
    Defer(Defer const&) = delete;
    ~Defer()
    {
        if (onExit)
            (*onExit)();
    }
    [[no_unique_address]] std::optional<F> onExit;
};

template<class F>
Defer(F) -> Defer<F>;

} // namespace di


#endif // INCLUDE_DI_DEFER_HPP
