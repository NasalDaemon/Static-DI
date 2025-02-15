#ifndef INCLUDE_DI_GRAPH_HPP
#define INCLUDE_DI_GRAPH_HPP

#include "di/context_fwd.hpp"
#include "di/cluster.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_STD_MODULE
#include <concepts>
#include <functional>
#endif

namespace di {

DI_MODULE_EXPORT
template<template<class> class RootCluster, IsRootContext Context = NullContext>
requires std::derived_from<RootCluster<Context>, Cluster>
using InlineGraph = RootCluster<Context>;

DI_MODULE_EXPORT
template<IsNodeHandle Cluster, IsRootContext Context = NullContext>
using Graph = InlineGraph<ToNodeWrapper<Cluster>::template Node, Context>;

namespace detail {
    struct OnGraphConstructedVisitor
    {
        static constexpr void operator()(auto& node)
        {
            if constexpr (requires { node.onGraphConstructed(); })
                node.onGraphConstructed();
        }
    };
}

DI_MODULE_EXPORT
template<std::invocable F>
constexpr std::invoke_result_t<F> constructGraph(F f)
{
    using Result = std::invoke_result_t<F>;
    static_assert(IsRootContext<ContextParameterOf<Result>>);
    static_assert(std::derived_from<Result, Cluster>);
    Result result = std::invoke(f);
    result.visit(detail::OnGraphConstructedVisitor{});
    return result;
}

} // namespace di


#endif // INCLUDE_DI_GRAPH_HPP
