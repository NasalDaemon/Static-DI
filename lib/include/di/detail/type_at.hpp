#ifndef INCLUDE_DI_DETAIL_TYPE_AT_HPP
#define INCLUDE_DI_DETAIL_TYPE_AT_HPP

#include "di/macros.hpp"

#if DI_CPP_VER > 202302L and __cpp_pack_indexing >= 202311L
#   define DI_TYPE_AT_VER 1
#elif DI_COMPILER_GNU
#   define DI_TYPE_AT_VER 2
#endif

#if !DI_STD_MODULE
#include <cstdint>
#   if !DI_TYPE_AT_VER
#   include <type_traits>
#   include <utility>
#   endif
#endif

namespace di::detail {

#if DI_TYPE_AT_VER == 1

template<std::size_t I, class... Ts>
using TypeAt = Ts...[I];

#elif DI_TYPE_AT_VER == 2

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
