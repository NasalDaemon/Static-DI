#ifndef INCLUDE_DI_DETAIL_CONCEPTS_HPP
#define INCLUDE_DI_DETAIL_CONCEPTS_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di::detail {

template<template<class> class Template>
using TakesUnaryClassTemplate = void;

template<template<class, auto...> class Template>
using TakesUnaryClassPackedAutoTemplate = void;

template<class...>
inline constexpr bool alwaysTrue = true;
template<class...>
inline constexpr bool alwaysFalse = false;

template<class...>
using Void = void;

template<class T>
requires std::is_void_v<T>
using VoidOnly = void;

template<class... Ts>
using AllVoid = Void<VoidOnly<Ts>...>;

} // namespace di::detail


#endif // INCLUDE_DI_DETAIL_CONCEPTS_HPP
