#ifndef INCLUDE_DI_FUNCTION_HPP
#define INCLUDE_DI_FUNCTION_HPP

#include "di/detail/select.hpp"

#include "di/compiler.hpp"
#include "di/empty_types.hpp"
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <memory>
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct FunctionPolicy
{
    static constexpr bool movable = true;
    bool copyable;
    bool mutableCall;
    bool constCall;
    auto operator<=>(FunctionPolicy const&) const = default;
};

DI_MODULE_EXPORT
template<class F, class... Args>
concept StatelessInvocable = IsStateless<F> and std::invocable<F, Args...>;

// Lightweight function wrapper that can be used to store any callable object in a single pointer
// It supports mutable and/or immutable calls, and can be made copyable
// Takes a function signature as a template parameter, e.g. `Function<void(int)>`
// Stores a single pointer: `sizeof(Function<void(int)>) == sizeof(void*)`
// Default policy is a move-only object with only a mutable call
DI_MODULE_EXPORT
template<class F, FunctionPolicy = FunctionPolicy{.copyable=false, .mutableCall=true, .constCall=false}>
struct Function;

template<class R, class... Args, FunctionPolicy Policy_>
struct Function<R(Args...), Policy_>
{
    static constexpr FunctionPolicy Policy = Policy_;
    static_assert(Policy.constCall or Policy.mutableCall);

    Function() = default;
    Function(Function&&) = default;
    Function& operator=(Function&&) = default;

    Function(Function const& other) requires (Policy.copyable)
        : callable{other.callable ? other.callable->copy(other.callable.get()) : nullptr}
    {}
    Function& operator=(Function const& other) requires (Policy.copyable)
    {
        callable.reset(other.callable ? other.callable->copy(other.callable.get()) : nullptr);
        return *this;
    }

    template<std::invocable<Args...> F>
    requires (not std::same_as<std::remove_cvref_t<F>, Function>)
    constexpr Function(F&& f) : callable{new Callable<std::remove_cvref_t<F>>(DI_FWD(f))}
    {}

    constexpr R operator()(auto&&... args) requires (Policy.mutableCall)
    {
        return callable->mutableFunction(callable.get(), DI_FWD(args)...);
    }

    constexpr R operator()(auto&&... args) const requires (Policy.constCall)
    {
        return callable->immutableFunction(callable.get(), DI_FWD(args)...);
    }

    constexpr operator bool() const { return callable != nullptr; }

    constexpr void reset() { callable.reset(); }

private:
    struct CallableBase
    {
        using MutableFunction = R(*)(CallableBase*, Args...);
        using ImmutableFunction = R(*)(CallableBase const*, Args...);
        using Destroy = void(*)(CallableBase*);
        using Copy = CallableBase*(*)(CallableBase const*);

        Destroy destroy;

        [[no_unique_address]]
        detail::EmptyIf<not Policy.mutableCall, MutableFunction> mutableFunction{};

        [[no_unique_address]]
        detail::EmptyIf<not Policy.constCall, ImmutableFunction> immutableFunction{};

        [[no_unique_address]]
        detail::EmptyIf<not Policy.copyable, Copy> copy{};
    };

    template<class F>
    struct Callable : CallableBase
    {
        constexpr explicit Callable(auto&& f)
            : CallableBase{.destroy{[](CallableBase* base) -> void { delete static_cast<Callable*>(base); }}}
            , f(DI_FWD(f))
        {
            if constexpr (Policy.mutableCall)
            {
                this->mutableFunction =
                    [](CallableBase* base, Args... args) -> R
                    {
                        if constexpr (compiler < gcc(15))
                            return std::invoke(static_cast<Callable*>(base)->f, DI_FWD(args)...);
                        else
                            return std::invoke_r<R>(static_cast<Callable*>(base)->f, DI_FWD(args)...);
                    };
            }
            if constexpr (Policy.constCall)
            {
                this->immutableFunction =
                    [](CallableBase const* base, Args... args) -> R
                    {
                        if constexpr (compiler < gcc(15))
                            return std::invoke(static_cast<Callable const*>(base)->f, DI_FWD(args)...);
                        else
                            return std::invoke_r<R>(static_cast<Callable const*>(base)->f, DI_FWD(args)...);
                    };
            }
            if constexpr (Policy.copyable)
            {
                this->copy =
                    [](CallableBase const* base) -> CallableBase*
                    {
                        return new Callable(*static_cast<Callable const*>(base));
                    };
            }
        }

        [[no_unique_address]] F f;
    };

    struct Deleter
    {
        constexpr void operator()(CallableBase* p) const { p->destroy(p); }
    };
    std::unique_ptr<CallableBase, Deleter> callable;
};

} // namespace di

#endif // INCLUDE_DI_FUNCTION_HPP
