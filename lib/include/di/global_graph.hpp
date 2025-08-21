#ifndef INCLUDE_DI_GLOBAL_HPP
#define INCLUDE_DI_GLOBAL_HPP

#include "di/detail/as_ref.hpp"

#include "di/context.hpp"
#include "di/cluster.hpp"
#include "di/global_trait.hpp"
#include "di/graph.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#include <utility>
#include <memory>
#endif

namespace di {

namespace detail {
    template<class GlobalNode, class Root>
    struct GlobalHost;

    template<IsNodeHandle GlobalNodeHandle, class Root>
    struct GlobalHost<GlobalNodeHandle*, Root>
    {
        using Type = ToNodeWrapper<GlobalNodeHandle>::template Node<GetRootContext<Root>>;
        using Member = ToNodeState<Type>*;
    };

    template<IsNodeHandle GlobalNodeHandle, class Root>
    struct GlobalHost<GlobalNodeHandle, Root>
    {
        using Type = ToNodeWrapper<GlobalNodeHandle>::template Node<GetRootContext<Root>>;
        using Member = ToNodeState<Type>;
    };

    template<class GlobalNodeHandle, class Root, class RootContext>
    struct GlobalInfo : RootContext::Info
    {
        using GlobalNode = GlobalHost<GlobalNodeHandle, Root>::Type;
    };

    template<IsNodeHandle MainCluster, class GlobalNodeHandle, class Root/* = void*/>
    struct GraphWithGlobalNode
    {
        using GlobalNode = GlobalHost<GlobalNodeHandle, Root>::Type;

        template<class RootContext>
        struct Node : di::Cluster
        {
            struct MainContext : di::Context<Node, MainCluster>
            {
                using Info = GlobalInfo<GlobalNodeHandle, Root, RootContext>;
                template<class N>
                static constexpr auto& getGlobalNode(N& node)
                {
                    auto memPtr = getNodePointer(AdlTag<MainContext>{});
                    auto& global = memPtr.getClassFromMember(node).global;
                    if constexpr (std::is_pointer_v<decltype(Node::global)>)
                        return std::forward_like<N&>(*global);
                    else
                        return std::forward_like<N&>(global);
                }
            };

            template<class T>
            static auto resolveLink(T) -> ResolvedLink<MainContext, T>;

            [[no_unique_address]] GlobalHost<GlobalNodeHandle, Root>::Member global;
            DI_NODE(MainContext, main)

            constexpr auto* operator->(this auto& self)
            {
                return std::addressof(self.main);
            }

            template<class Self>
            constexpr void visit(this Self& self, auto&& visitor)
            {
                if constexpr (std::is_pointer_v<decltype(Node::global)>)
                    std::forward_like<Self&>(*self.global).visit(visitor);
                else
                    self.global.visit(visitor);
                self.main.visit(visitor);
            }
        };
    };
}

DI_MODULE_EXPORT
template<IsNodeHandle MainCluster, class GlobalNode, class Root = void>
requires IsNodeHandle<std::remove_pointer_t<GlobalNode>>
using GraphWithGlobal = Graph<detail::GraphWithGlobalNode<MainCluster, GlobalNode, Root>, Root>;

} // namespace di


#endif // INCLUDE_DI_GLOBAL_HPP
