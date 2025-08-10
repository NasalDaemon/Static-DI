#ifndef INCLUDE_DI_GLOBAL_CONTEXT_HPP
#define INCLUDE_DI_GLOBAL_CONTEXT_HPP

#include "di/context_fwd.hpp"
#include "di/global_trait.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"

namespace di {

DI_MODULE_EXPORT
template<class Context>
concept ContextHasGlobal = IsContext<Context> and requires {
    typename Context::Info::GlobalNode;
};

DI_MODULE_EXPORT
template<class Context, class GlobalTrait>
concept ContextHasGlobalTrait = ContextHasGlobal<Context> and IsGlobalTrait<GlobalTrait> and HasTrait<typename Context::Info::GlobalNode, typename GlobalTrait::Trait>;

} // namespace di


#endif // INCLUDE_DI_GLOBAL_CONTEXT_HPP
