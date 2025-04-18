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
        struct Inner : di::Context<Node, NodeHandle>
        {
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

        constexpr void visit(this auto& self, auto const& f)
        {
            self.node.visit(f);
        }
    };
};

}


#endif // INCLUDE_DI_NARROW_HPP
