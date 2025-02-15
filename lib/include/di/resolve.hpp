#ifndef INCLUDE_DI_RESOLVE_HPP
#define INCLUDE_DI_RESOLVE_HPP

#include "di/link.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

namespace di {

namespace detail {

    template<class T, class Trait>
    struct ResolveTypes
    {
        using type = NodeTypes<T, Trait>;
    };

    template<class T, class Trait>
    requires HasLink<T, Trait>
    struct ResolveTypes<T, Trait>
    {
        using Target = ResolveLink<T, Trait>;
        using type = ResolveTypes<ContextToNode<typename Target::Context>, typename Target::Trait>::type;
    };

    // Delegate resolution to the parent cluster's context
    template<class T, class Trait>
    requires LinksToParent<T, Trait>
    struct ResolveTypes<T, Trait>
    {
        using Target = ResolveLink<T, Trait>;
        using type = ResolveTypes<typename Target::Context, typename Target::Trait>::type;
    };

} // namespace detail

DI_MODULE_EXPORT
template<IsContext Context, IsTrait Trait>
requires detail::HasLink<Context, Trait>
using ResolveTypes = detail::ResolveTypes<Context, Trait>::type;

DI_MODULE_EXPORT
template<IsContext Context>
using ResolveRoot = Context::Root;

DI_MODULE_EXPORT
template<IsContext Context>
using ResolveInfo = Context::Info;

} // namespace di


#endif // INCLUDE_DI_RESOLVE_HPP
