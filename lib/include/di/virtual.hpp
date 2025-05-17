#ifndef INCLUDE_DI_VIRTUAL_HPP
#define INCLUDE_DI_VIRTUAL_HPP

#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"
#include "di/detail/select.hpp"
#include "di/context.hpp"
#include "di/empty_types.hpp"
#include "di/factory.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/traits.hpp"
#include "di/traits_fwd.hpp"

#if !DI_IMPORT_STD
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
struct IDestructible
{
    virtual ~IDestructible() = default;
};

DI_MODULE_EXPORT
struct [[nodiscard, maybe_unused]] KeepAlive
{
    explicit constexpr KeepAlive(std::unique_ptr<IDestructible> m) : m(std::move(m)) {}
    KeepAlive() = default;
    KeepAlive(KeepAlive&&) = default;
    KeepAlive& operator=(KeepAlive&&) = default;

private:
    std::unique_ptr<IDestructible> m;
};

namespace detail {
    struct INodeBase;
    struct IsVirtualContextTag{};

    template<IsNodeWrapper T, template<class> class InnerContext>
    auto toVirtualNodeImpl() -> T::template Node<CompressContext<InnerContext<T>>>;
    template<IsNode T, template<class> class>
    auto toVirtualNodeImpl() -> T;
}

DI_MODULE_EXPORT
template<class T>
concept IsInterface = std::derived_from<T, detail::INodeBase> and requires {
    typename T::Traits;
};

DI_MODULE_EXPORT
template<class Context>
concept IsVirtualContext = IsContext<Context> and requires {
    { Context::isVirtualContext(detail::IsVirtualContextTag()) } -> std::same_as<detail::Decompress<Context>>;
};

struct detail::INodeBase : Node
{
    template<IsNodeHandle T, class Self>
    requires IsVirtualContext<ContextOf<Self>>
    constexpr auto exchangeImpl(this Self& self, auto&&... args)
    {
        static_assert(not std::is_const_v<Self>);
        return ContextOf<Self>::template exchangeImpl<T>(std::addressof(self), DI_FWD(args)...);
    }

    virtual void onGraphConstructed() {}
};

DI_MODULE_EXPORT
struct INode : virtual detail::INodeBase
{};

DI_MODULE_EXPORT
template<IsInterface... Interfaces>
struct Virtual
{
    static_assert(sizeof...(Interfaces) != 0);

    template<class Context>
    struct Node : di::Node
    {
    private:
        struct ImplBase : IDestructible
        {
        protected:
            friend Node;
            constexpr ImplBase(Node* virtualHost)
                : virtualHost(virtualHost)
            {}
            Node* virtualHost;
        };

        template<class ImplNode>
        struct InnerContext : Context
        {
            static InnerContext isVirtualContext(detail::IsVirtualContextTag);

            template<IsNodeHandle T, class Current>
            requires (not std::is_const_v<Current>)
            static constexpr auto exchangeImpl(Current* current, auto&&... args)
            {
                return Impl<ImplNode>::getVirtualHost(current)->template exchangeImpl<T>(current, DI_FWD(args)...);
            }

            template<class N, IsTrait Trait>
            requires detail::HasLink<Context, Trait>
            constexpr auto getNode(this Context virtualContext, N& impl, Trait trait)
            {
                Node* virtualHost = Impl<ImplNode>::getVirtualHost(std::addressof(impl));
                return virtualContext.getNode(std::forward_like<N&>(*virtualHost), trait);
            }
        };

        template<IsNodeHandle T>
        using ImplOf = decltype(detail::toVirtualNodeImpl<T, InnerContext>());

        template<IsNodeHandle ImplNode>
        struct Impl final : ImplBase
        {
            explicit constexpr Impl(Node* virtualHost, auto&&... args)
                : ImplBase{virtualHost}
                , impl{DI_FWD(args)...}
            {}

        private:
            friend Node;
            ImplOf<ImplNode> impl;

            static constexpr Node* getVirtualHost(ImplOf<ImplNode> const* p)
            {
                auto const& self = p->*detail::reverseMemberPointer(&Impl::impl);
                return self.virtualHost;
            }
        };

        template<class Trait>
        using InterfaceOf = detail::SelectIf<
            detail::NodeTraitsHasTraitPred<Trait>,
            Interfaces...
        >;

        template<class Interface>
        struct AsInterface;

        template<class Trait>
        requires (... || detail::TraitsHasTrait<typename Interfaces::Traits, Trait>)
        struct Resolver
        {
            using TraitInterface = InterfaceOf<Trait>;
            using Types = TraitInterface::Traits::template ResolveTypes<Trait>;
            using Interface = AsInterface<TraitInterface>;
            DI_ASSERT_IMPLEMENTS(TraitInterface, Types, Trait);
        };

    public:
        using Traits = di::TraitsTemplate<Node, Resolver>;

        template<IsNodeHandle T>
        requires (... && std::derived_from<ImplOf<T>, Interfaces>)
        explicit(false) constexpr Node(std::in_place_type_t<T>, auto&&... args)
        {
            [[maybe_unused]] auto ignored = init<T>(DI_FWD(args)...);
        }

        template<std::invocable<Constructor<Node>> F>
        requires std::same_as<Node, std::invoke_result_t<F, Constructor<Node>>>
        explicit constexpr Node(WithFactory, F factory)
            : Node(factory(Constructor<Node>()))
        {}

        template<IsNodeHandle T>
        requires (... && std::derived_from<ImplOf<T>, Interfaces>)
        constexpr ImplOf<T>& emplace(auto&&... args)
        {
            auto [next, prev] = init<T>(DI_FWD(args)...);
            return next->impl;
        }

        Node(Node const&) = delete;
        constexpr Node(Node&& other)
            : interfaces(other.interfaces)
            , implBase(std::move(other.implBase))
        {
            other.interfaces = {};
            if (implBase)
                implBase->virtualHost = this;
        }

        constexpr void onGraphConstructed()
        {
            get<0>(interfaces)->onGraphConstructed();
        }

    protected:
        std::tuple<Interfaces*...> interfaces;
        std::unique_ptr<ImplBase> implBase;

        template<IsNodeHandle T>
        requires (... && std::derived_from<ImplOf<T>, Interfaces>)
        [[nodiscard]] constexpr auto init(auto&&... args)
        {
            Impl<T>* p = new Impl<T>(this, DI_FWD(args)...);
            // Updates pointers if new didn't throw
            interfaces = {static_cast<Interfaces*>(std::addressof(p->impl))...};
            return std::pair(p, std::exchange(implBase, std::unique_ptr<ImplBase>(p)));
        }

        template<IsNodeHandle T>
        constexpr auto exchangeImpl(auto* current, auto&&... args)
        {
            if (get<0>(interfaces) != current)
                throw std::runtime_error("Pointers not matching");

            auto [next, prev] = init<T>(DI_FWD(args)...);
            return Exchanged(next->impl, std::move(prev));
        }

        template<class Next>
        struct [[nodiscard, maybe_unused]] Exchanged
        {
            constexpr Exchanged(Next& next, std::unique_ptr<ImplBase> previous)
                : next(next)
                , previous(std::move(previous))
            {}

            // Do not allow reference to next to escape block scope
            // to avoid implicitly holding it somewhere as a dangling reference
            Exchanged(Exchanged&&) = delete;

            // Allow extension of previous lifetime beyond the current block
            constexpr KeepAlive keepAlive() { return KeepAlive(std::move(previous)); }
            constexpr operator KeepAlive() { return keepAlive(); }

            // Access to next only allowed when ExchangeHandle is an lvalue to ensure that
            // previous lifetime has at least been extended to the block scope of the exchangeImpl caller
            constexpr Next& getNext() & { return next; }
        private:
            Next& next;
            std::unique_ptr<ImplBase> previous;
        };
    };
};

template<IsInterface... Interfaces>
template<class Context>
template<class Interface>
struct Virtual<Interfaces...>::Node<Context>::AsInterface : Node
{
    template<class Self>
    constexpr decltype(auto) apply(this Self& self, auto&&... args)
    {
        return std::forward_like<Self&>(*get<Interface*>(self.interfaces)).apply(DI_FWD(args)...);
    }
};

}

#endif // INCLUDE_DI_VIRTUAL_HPP
