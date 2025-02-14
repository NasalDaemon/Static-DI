#ifndef INCLUDE_DI_DETAIL_TYPE_AT_HPP
#define INCLUDE_DI_DETAIL_TYPE_AT_HPP

#include "di/macros.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#include <utility>
#endif

namespace di::detail {

#if defined(DI_COMPILER_CLANG) or defined(DI_COMPILER_GCC)

template<std::size_t I, class... Ts>
requires (I < sizeof...(Ts))
using TypeAt = __type_pack_element<I, Ts...>;

#else

struct Sink
{
    consteval Sink(auto) {}
};

template<class... Sinks, class T>
T getTypeAt2(Sinks..., std::type_identity<T>, auto...);

template<class... Ts, std::size_t... Is>
auto getTypeAt1(std::index_sequence<Is...>) -> decltype(getTypeAt2<decltype(Sink(Is))...>(std::type_identity<Ts>()...));

template<std::size_t I, class... Ts>
requires (I < sizeof...(Ts))
using TypeAt = decltype(getTypeAt1<Ts...>(std::make_index_sequence<I>{}));

#endif

}


#endif // INCLUDE_DI_DETAIL_TYPE_AT_HPP
