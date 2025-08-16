#ifndef INCLUDE_DI_DEFER_HPP
#define INCLUDE_DI_DEFER_HPP

#include "di/compiler.hpp"
#include "di/function.hpp"
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <optional>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
template<class F = Function<void()>>
requires std::invocable<F> or (compiler < gcc(15))
struct [[nodiscard, maybe_unused]] Defer
{
    Defer() = default;
    Defer(auto&&... args)
        : onExit(std::in_place, DI_FWD(args)...)
    {}
#if DI_COMPILER_LT(GCC, 15)
    Defer(F&& f)
        : onExit(std::in_place, std::move(f))
    {}
#endif
    template<class F2>
    Defer(Defer<F2> const&) = delete;

    template<class F2>
    Defer(Defer<F2>&& other)
        : onExit(std::exchange(other.onExit, std::nullopt))
    {}

    template<class F2>
    Defer& operator=(Defer<F2>&& other)
    {
        onExit = std::exchange(other.onExit, std::nullopt);
        return *this;
    }

    ~Defer()
    {
        if (onExit)
            (*onExit)();
    }

private:
    template<class F2>
    requires std::invocable<F2> or (compiler < gcc(15))
    friend struct Defer;

    [[no_unique_address]] std::optional<F> onExit;
};

template<class F>
Defer(F) -> Defer<F>;

} // namespace di


#endif // INCLUDE_DI_DEFER_HPP
