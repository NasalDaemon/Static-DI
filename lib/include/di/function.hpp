#ifndef INCLUDE_DI_FUNCTION_HPP
#define INCLUDE_DI_FUNCTION_HPP

#include "di/detail/select.hpp"
#include "di/compiler.hpp"
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
    bool immutableCall;
    auto operator<=>(FunctionPolicy const&) const = default;
};

// Lightweight function wrapper that can be used to store any callable object in a single pointer
// It supports mutable and/or immutable calls, and can be made copyable
// Takes a function signature as a template parameter, e.g. `Function<void(int)>`
// Stores a single pointer: sizeof(Function<void(int)>) == sizeof(void*)
// Default policy is a move-only object with only a mutable call
DI_MODULE_EXPORT
template<class F, FunctionPolicy = FunctionPolicy{.copyable=false, .mutableCall=true, .immutableCall=false}>
struct Function;

namespace detail {

    template<class F, FunctionPolicy Policy>
    struct CallableBase;

    template<class R, class... Args, FunctionPolicy Policy>
    struct CallableBase<R(Args...), Policy>
    {
        static_assert(Policy.immutableCall or Policy.mutableCall);

        using MutableFunction = R(*)(CallableBase*, Args...);
        using ImmutableFunction = R(*)(CallableBase const*, Args...);
        using Destroy = void(*)(CallableBase*);
        using Copy = CallableBase*(*)(CallableBase const*);

        Destroy destroy;

        [[no_unique_address]]
        detail::EmptyIf<not Policy.mutableCall, MutableFunction> mutableFunction{};

        [[no_unique_address]]
        detail::EmptyIf<not Policy.immutableCall, ImmutableFunction> immutableFunction{};

        [[no_unique_address]]
        detail::EmptyIf<not Policy.copyable, Copy> copy{};

        template<class F>
        struct Callable;
    };
    template<class R, class... Args, FunctionPolicy Policy>
    template<class F>
    struct CallableBase<R(Args...), Policy>::Callable : CallableBase
    {
        [[no_unique_address]] F f;
    };

} // namespace detail

template<class R, class... Args, FunctionPolicy Policy>
struct Function<R(Args...), Policy>
{
    Function() = default;
    Function(Function&&) = default;
    Function& operator=(Function&&) = default;

    Function(Function const& other) requires (Policy.copyable)
    {
        if (other.callable)
            callable.reset(other.callable->copy(other.callable.get()));
    }
    Function& operator=(Function const& other) requires (Policy.copyable)
    {
        callable.reset(other.callable ? other.callable->copy(other.callable.get()) : nullptr);
        return *this;
    }

    template<std::invocable<Args...> F>
    constexpr Function(F&& f) : callable{makeCallable<F>(DI_FWD(f))}
    {}

    constexpr R operator()(auto&&... args) requires (Policy.mutableCall)
    {
        return callable->mutableFunction(callable.get(), DI_FWD(args)...);
    }

    constexpr R operator()(auto&&... args) const requires (Policy.immutableCall)
    {
        return callable->immutableFunction(callable.get(), DI_FWD(args)...);
    }

    constexpr operator bool() const { return callable; }

private:
    using CallableBase = detail::CallableBase<R(Args...), Policy>;
    template<class F>
    using Callable = CallableBase::template Callable<std::remove_cvref_t<F>>;
    struct Deleter
    {
        constexpr void operator()(CallableBase* p) const { p->destroy(p); }
    };
    std::unique_ptr<CallableBase, Deleter> callable;

    template<class F>
    static constexpr CallableBase* makeCallable(F&& f)
    {
        Callable<F>* p = new Callable<F>{
            {
                .destroy{
                    [](CallableBase* base) -> void
                    {
                        delete static_cast<Callable<F>*>(base);
                    }
                },
            },
            DI_FWD(f),
        };
        if constexpr (Policy.mutableCall)
        {
            p->mutableFunction =
                [](CallableBase* base, Args... args) -> R
                {
                    if constexpr (compiler < gcc(15))
                        return std::invoke(static_cast<Callable<F>*>(base)->f, DI_FWD(args)...);
                    else
                        return std::invoke_r<R>(static_cast<Callable<F>*>(base)->f, DI_FWD(args)...);
                };
        }
        if constexpr (Policy.immutableCall)
        {
            p->immutableFunction =
                [](CallableBase const* base, Args... args) -> R
                {
                    if constexpr (compiler < gcc(15))
                        return std::invoke(static_cast<Callable<F> const*>(base)->f, DI_FWD(args)...);
                    else
                        return std::invoke_r<R>(static_cast<Callable<F> const*>(base)->f, DI_FWD(args)...);
                };
        }
        if constexpr (Policy.copyable)
        {
            p->copy =
                [](CallableBase const* base) -> CallableBase*
                {
                    return new Callable<F>(*static_cast<Callable<F> const*>(base));
                };
        }
        return p;
    }
};

} // namespace di

#endif // INCLUDE_DI_FUNCTION_HPP
