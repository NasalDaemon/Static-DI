#ifndef INCLUDE_DI_NARROW_HPP
#define INCLUDE_DI_NARROW_HPP

#include "di/cluster.hpp"
#include "di/context.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/trait.hpp"

namespace di {

DI_MODULE_EXPORT
template<IsNodeHandle NodeHandle, IsTrait... Traits>
struct Narrow
{
    template<class Context>
    struct Node : Cluster
    {
        static constexpr bool isUnary() { return decltype(node)::isUnary(); }

        struct Inner : di::Context<Node, NodeHandle>
        {
            static constexpr std::size_t Depth = Context::Depth;

            template<class T>
            requires detail::HasLink<Context, T>
            static auto resolveLink(T) -> ResolvedLink<Context, T>;
        };
        DI_NODE(Inner, node);

        // Narrow exposed traits
        template<class T>
        requires (... || MatchesTrait<T, Traits>)
        static auto resolveLink(T) -> ResolvedLink<Inner, T>;

        constexpr decltype(auto) operator->(this auto& self)
        {
            if constexpr (IsNode<ContextToNode<Inner>>)
                return (self.node);
            else
                return std::addressof(self.node);
        }

        constexpr void visit(this auto& self, auto&& visitor)
        {
            self.node.visit(DI_FWD(visitor));
        }
    };
};

}


#endif // INCLUDE_DI_NARROW_HPP
