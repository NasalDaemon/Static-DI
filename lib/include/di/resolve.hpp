#ifndef INCLUDE_DI_RESOLVE_HPP
#define INCLUDE_DI_RESOLVE_HPP

#include "di/global_context.hpp"
#include "di/global_trait.hpp"
#include "di/link.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

namespace di {

namespace detail {

    // T has no link, so it is the target node
    template<class T, class Trait_>
    struct ResolveTraitT
    {
        using type = ResolveTraitT;
        using Node = T;
        using Trait = Trait_;

        using Types = Node::Traits::template ResolveTypes<Trait>;
    };

    // GlobalTrait is a global trait that can be resolved in contexts with a global node
    template<class T, IsGlobalTrait GlobalTrait>
    struct ResolveTraitT<T, GlobalTrait>
    {
        static_assert(ContextHasGlobalTrait<T, GlobalTrait>,
                      "Global trait can only be resolved in graphs that have a global node");
        using Trait = GlobalTrait::Trait;
        using type = ResolveTraitT<typename T::Info::GlobalNode, Trait>::type;
    };

    // T is a context with a link to a sibling node's context
    template<class T, class Trait>
    requires HasLink<T, Trait>
    struct ResolveTraitT<T, Trait>
    {
        using Target = ResolveLink<T, Trait>;
        using type = ResolveTraitT<ContextToNode<typename Target::Context>, typename Target::Trait>::type;
    };

    // T links to parent context
    template<class T, class Trait>
    requires LinksToParent<T, Trait>
    struct ResolveTraitT<T, Trait>
    {
        using Target = ResolveLink<T, Trait>;
        using type = ResolveTraitT<typename Target::Context, typename Target::Trait>::type;
    };

    template<class T, class Trait>
    using ResolveTrait = ResolveTraitT<T, Trait>::type;

} // namespace detail

DI_MODULE_EXPORT
template<class Node, IsTrait Trait>
requires detail::HasLink<ContextOf<Node>, Trait> and NodeDependencyAllowed<Node, Trait>
using ResolveTypes = detail::ResolveTrait<ContextOf<Node>, Trait>::Types;

DI_MODULE_EXPORT
template<IsContext Context>
using ResolveRoot = Context::Root;

DI_MODULE_EXPORT
template<IsContext Context>
using ResolveInfo = Context::Info;

} // namespace di


#endif // INCLUDE_DI_RESOLVE_HPP
