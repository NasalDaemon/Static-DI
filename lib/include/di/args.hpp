#ifndef INCLUDE_DI_ARGS_HPP
#define INCLUDE_DI_ARGS_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <tuple>
#endif

namespace di {

DI_MODULE_EXPORT
template<class Tag_, class... Ts>
struct Args
{
    using Tag = Tag_;

    constexpr explicit Args(Ts&&... args) : args(DI_FWD(args)...) {}
    // Args should always be passed around as a temporary reference, no need for relocation
    Args(Args&&) = delete;
    Args(Args const&) = delete;

    template<class T>
    T make() const
    {
        return std::make_from_tuple<T>(std::move(args));
    }

    template<class T>
    auto&& get() const
    {
        return std::get<T&&>(args);
    }

private:
    std::tuple<Ts&&...> args;
};

DI_MODULE_EXPORT
template<class Tag, class... Ts>
constexpr Args<Tag, Ts...> args(Ts&&... args)
{
    return Args<Tag, Ts...>{DI_FWD(args)...};
}

namespace detail {
    template<class Tag, class... Ts>
    constexpr void isArgs(Args<Tag, Ts...> const&);
}

DI_MODULE_EXPORT
template<class T>
concept IsArgs = requires(T t) { detail::isArgs(t); };

DI_MODULE_EXPORT
template<class T, class Tag>
concept IsArgsOf = IsArgs<T> and std::same_as<typename std::remove_reference_t<T>::Tag, Tag>;

}

#endif // INCLUDE_DI_ARGS_HPP
