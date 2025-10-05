#ifndef INCLUDE_DI_TRAITS_DYNAMIC_NODE_HPP
#define INCLUDE_DI_TRAITS_DYNAMIC_NODE_HPP

#include "di/traits/dynamic_node.hxx"

#include "di/macros.hpp"
#include "di/union_fwd.hpp"
#include "di/virtual_fwd.hpp"

namespace di {

DI_MODULE_EXPORT
template<class Context>
concept IsDynamicContext = IsContext<Context> and (IsVirtualContext<Context> or IsUnionContext<Context>);

} // namespace di

#endif // INCLUDE_DI_TRAITS_DYNAMIC_NODE_HPP
