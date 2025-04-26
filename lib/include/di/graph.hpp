#ifndef INCLUDE_DI_GRAPH_HPP
#define INCLUDE_DI_GRAPH_HPP

#include "di/context_fwd.hpp"
#include "di/cluster.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <functional>
#include <type_traits>
#endif

namespace di {

namespace detail {
    template<class T>
    auto getRootContext() -> RootContext<T>;
    template<class T>
    requires std::is_void_v<T>
    auto getRootContext() -> NullContext;

    template<class T>
    using GetRootContext = decltype(getRootContext<T>());
}

DI_MODULE_EXPORT
template<template<class> class RootCluster, class Root = void>
requires std::derived_from<RootCluster<detail::GetRootContext<Root>>, Cluster>
using InlineGraph = RootCluster<detail::GetRootContext<Root>>;

DI_MODULE_EXPORT
template<IsNodeHandle Cluster, class Root = void>
using Graph = InlineGraph<ToNodeWrapper<Cluster>::template Node, Root>;

DI_MODULE_EXPORT
template<std::invocable F>
constexpr std::invoke_result_t<F> constructGraph(F f)
{
    using Result = std::invoke_result_t<F>;
    static_assert(std::derived_from<Result, Cluster>);
    Result result = std::invoke(f);
    result.onConstructed();
    return result;
}

} // namespace di


#endif // INCLUDE_DI_GRAPH_HPP
