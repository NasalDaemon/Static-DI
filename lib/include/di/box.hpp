#ifndef INCLUDE_DI_BOX_HPP
#define INCLUDE_DI_BOX_HPP

#include "di/detail/cast.hpp"
#include "di/adapt.hpp"
#include "di/cluster.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/virtual.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <memory>
#endif

namespace di {

namespace detail {

    template<class OuterNode>
    struct OuterNodeProxy
    {
        template<class Context>
        struct Node : di::Node
        {
            template<class Trait>
            struct WithTrait;

            template<class Trait>
            // Types not used as this is behind a facade which already concretises the types
            using Resolver = ResolvedTrait<WithTrait<Trait>, EmptyTypes>;

            using Traits = di::TraitsTemplate<Node, Resolver>;

            explicit constexpr Node(OuterNode* outerNode) : outerNode(outerNode) {}

            OuterNode* outerNode;
        };
    };

    template<class OuterNode>
    template<class Context>
    template<class Trait>
    struct OuterNodeProxy<OuterNode>::Node<Context>::WithTrait : Node
    {
        template<class Self>
        DI_INLINE constexpr decltype(auto) impl(this Self& self, auto&&... args)
        {
            return std::forward_like<Self&>(*self.outerNode).getNode(Trait{}).impl(DI_FWD(args)...);
        }
    };

    template<IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade, IsInterface... OutInterfaces>
    requires (IsNodeHandle<OutFacade> and sizeof...(OutInterfaces) > 0)
          or (std::is_void_v<OutFacade> and sizeof...(OutInterfaces) == 0)
    struct Boxed
    {
        template<IsRootContext Context>
        struct Node : di::Cluster
        {
            struct Main_;
            struct InFacade_;
            struct OutVirtual_;

            constexpr auto* asInterface(this auto& self) { return std::addressof(self.inFacade); }

            struct Main_ : di::Context<Node, Main>
            {
                template<class Trait>
                static ResolvedLink<OutVirtual_, Trait> resolveLink(Trait);
            };

            struct InFacade_ : di::Context<Node, InFacade>
            {
                template<class Trait>
                static ResolvedLink<Main_, Trait> resolveLink(Trait);
            };

            struct OutVirtual_ : di::Context<Node, di::Virtual<OutInterfaces...>>
            {};

            DI_NODE(Main_, main)

            detail::ToVirtualNodeImpl<InFacade, InFacade_> inFacade{};
            friend consteval auto getNodePointer(di::AdlTag<InFacade_>) { return &Node::inFacade; }
            static_assert(IsInterface<decltype(inFacade)>);

            DI_NODE(OutVirtual_, outVirtual)

            template<class OutNode, class... Ts>
            requires (... and not IsArgs<Ts>)
            constexpr explicit Node(OutNode* outNode, Ts&&... args)
                : main{DI_FWD(args)...}
                , outVirtual{std::in_place_type<Adapt<OutNode>>, outNode}
            {}

            template<class OutNode, class... MArgs, class... IArgs>
            explicit constexpr Node(OutNode* outNode, Args<Main, MArgs...> const& margs, Args<InFacade, IArgs...> const& iargs)
                : main{margs.template get<MArgs>()...}
                , inFacade{iargs.template get<IArgs>()...}
                , outVirtual{std::in_place_type<Adapt<OutNode>>, outNode}
            {}


            template<class OutNode>
            void setVirtualHost(OutNode* outNode)
            {
                outVirtual->template emplace<Adapt<OutNode>>(outNode);
            }

        private:
            template<class OutNode>
            static constexpr auto getAdapt() -> di::Adapt<OuterNodeProxy<OutNode>, OutFacade>;

            template<class OutNode>
            requires std::is_void_v<OutFacade>
            static constexpr auto getAdapt() -> void;

            template<class OutNode>
            using Adapt = decltype(getAdapt<OutNode>());
        };
    };

} // namespace detail

DI_MODULE_EXPORT
template<IsRootContext Root, IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade = void, IsInterface... OutInterfaces>
struct BoxWithRoot
{
    using Graph = di::Graph<detail::Boxed<Main, InFacade, OutFacade, OutInterfaces...>, Root>;

    template<IsVirtualContext>
    using Node = Graph;

    template<IsRootContext NewRoot = NullContext>
    using WithRoot = BoxWithRoot<NewRoot, Main, InFacade, OutFacade, OutInterfaces...>;
};

DI_MODULE_EXPORT
template<IsRootContext Root, IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade = void, IsInterface... OutInterfaces>
inline constexpr std::in_place_type_t<BoxWithRoot<Root, Main, InFacade, OutFacade, OutInterfaces...>> boxWithRoot{};

DI_MODULE_EXPORT
template<IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade = void, IsInterface... OutInterfaces>
struct Box
{
    using Graph = di::Graph<detail::Boxed<Main, InFacade, OutFacade, OutInterfaces...>>;

    template<IsVirtualContext>
    using Node = Graph;

    template<IsRootContext Root = NullContext>
    using WithRoot = BoxWithRoot<Root, Main, InFacade, OutFacade, OutInterfaces...>;
};

namespace detail {
    template<IsRootContext Root, IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade, IsInterface... OutInterfaces>
    constexpr bool injectVirtualHost<BoxWithRoot<Root, Main, InFacade, OutFacade, OutInterfaces...>> = true;
    template<IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade, IsInterface... OutInterfaces>
    constexpr bool injectVirtualHost<Box<Main, InFacade, OutFacade, OutInterfaces...>> = true;
}

DI_MODULE_EXPORT
template<IsNodeHandle Main, IsNodeHandle InFacade, class OutFacade = void, IsInterface... OutInterfaces>
inline constexpr std::in_place_type_t<Box<Main, InFacade, OutFacade, OutInterfaces...>> box{};

}

#endif // INCLUDE_DI_BOX_HPP
