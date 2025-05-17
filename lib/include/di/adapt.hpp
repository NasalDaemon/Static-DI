#ifndef INCLUDE_DI_ADAPT_HPP
#define INCLUDE_DI_ADAPT_HPP

#include "di/detail/cast.hpp"
#include "di/cluster.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/virtual.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#include <memory>
#endif

namespace di {

DI_MODULE_EXPORT
template<IsNodeHandle Target, IsNodeHandle Facade>
struct Adapt
{
    template<IsVirtualContext Context>
    struct Node : di::Cluster
    {
        struct Facade_;
        struct Target_;

        template<IsTrait T>
        static ResolvedLink<Facade_, T> resolveLink(T);

        constexpr auto* asInterface(this auto& self) { return std::addressof(self.facade); }

        struct Facade_ : di::Context<Node, Facade>
        {
            static constexpr std::size_t Depth = Context::Depth;

            template<class Trait>
            static ResolvedLink<Target_, Trait> resolveLink(Trait);

            static Facade_ isVirtualContext(detail::IsVirtualContextTag);

            template<IsNodeHandle T, class N>
            requires (not std::is_const_v<N>)
            static constexpr auto exchangeImpl(N& facade, auto&&... args)
            {
                Node& cluster = facade.*detail::reverseMemberPointer(&Node::facade);
                return Context::template exchangeImpl<T>(cluster, DI_FWD(args)...);
            }
        };

        struct Target_ : di::Context<Node, Target>
        {
            static constexpr std::size_t Depth = Context::Depth;

            template<class Trait>
            requires detail::HasLink<Context, Trait>
            static ResolvedLink<Context, Trait> resolveLink(Trait);
        };

        detail::ToVirtualNodeImpl<Facade, Facade_> facade{};
        friend consteval auto getNodePointer(di::AdlTag<Facade_>) { return &Node::facade; }
        static_assert(IsInterface<decltype(facade)>);

        DI_NODE(Target_, target)
    };
};

}

#endif // INCLUDE_DI_ADAPT_HPP
