#ifndef INCLUDE_DI_COMBINE_HPP
#define INCLUDE_DI_COMBINE_HPP

#include "di/detail/select.hpp"
#include "di/cluster.hpp"
#include "di/context.hpp"
#include "di/link.hpp"
#include "di/trait.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#endif

namespace di {

namespace detail {

    template<class CombineNode, IsNodeHandle Node>
    struct CombinePart
    {
        struct Context : di::Context<CombineNode, Node>
        {
            template<class T>
            static auto resolveLink(T) -> ResolvedLink<typename Context::ParentContext, T>;
        };

        using State = ContextToNodeState<Context>;
        [[no_unique_address]] State node{};

        friend consteval State CombineNode::* getNodePointer(di::AdlTag<Context>)
        {
            return &CombinePart::node;
        }
    };

} // namespace detail

DI_MODULE_EXPORT
template<IsNodeHandle... Nodes>
struct Combine
{
    template<class Context>
    struct Node : Cluster, detail::CombinePart<Node<Context>, Nodes>...
    {
        template<IsTrait Trait>
        static auto resolveLink(Trait)
            -> ResolvedLink<
                detail::SelectIf<
                    ContextHasTraitPred<Trait>,
                    typename detail::CombinePart<Node, Nodes>::Context...
                >,
                Trait>;

        template<IsNodeHandle N>
        requires (... || std::is_same_v<N, Nodes>)
        auto& get(this auto& self)
        {
            return detail::upCast<detail::CombinePart<Node, N>>(self).node;
        }

        constexpr void visit(this auto& self, auto const& visitor)
        {
            (self.template get<Nodes>().visit(visitor), ...);
        }
    };
};

} // namespace di


#endif // INCLUDE_DI_COMBINE_HPP
