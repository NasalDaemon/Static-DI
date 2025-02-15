#ifndef INCLUDE_DI_DETAIL_CONCEPTS_HPP
#define INCLUDE_DI_DETAIL_CONCEPTS_HPP

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

} // namespace di::detail


#endif // INCLUDE_DI_DETAIL_CONCEPTS_HPP
