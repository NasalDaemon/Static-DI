#ifndef INCLUDE_DI_COUNT_HPP
#define INCLUDE_DI_COUNT_HPP

#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#if !DI_IMPORT_STD
#include <cstdint>
#include <utility>
#endif


namespace di {

DI_MODULE_EXPORT
constexpr std::size_t nodeCount(auto const& node)
{
    std::size_t count = 0;
    node.visit([&](IsNode auto&) { ++count; });
    return count;
}

DI_MODULE_EXPORT
template<class T>
concept IsUnary = T::isUnary();

DI_MODULE_EXPORT
template<class T>
using EnsureAny = T;

DI_MODULE_EXPORT
template<IsUnary Node>
using EnsureUnary = Node;

DI_MODULE_EXPORT
template<IsUnary Node>
requires std::is_empty_v<Node>
using EnsureUnaryStateless = Node;

DI_MODULE_EXPORT
template<class Node>
requires (not IsUnary<Node>)
using EnsureNonUnary = Node;

} // namespace di


#endif // INCLUDE_DI_COUNT_HPP
