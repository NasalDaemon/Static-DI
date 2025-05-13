#ifndef INCLUDE_DI_VIRTUAL_HPP
#define INCLUDE_DI_VIRTUAL_HPP

#include "di/detail/cast.hpp"
#include "di/detail/compress.hpp"
#include "di/detail/select.hpp"
#include "di/context.hpp"
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
struct INode;

DI_MODULE_EXPORT
template<class T>
concept IsInterface = std::is_base_of_v<INode, T> and requires {
    typename T::Traits;
};

DI_MODULE_EXPORT
template<IsInterface... Interfaces>
struct Virtual;

namespace detail {
    struct IsVirtualContextTag{};
    template<class Context>
    concept IsVirtualContext = IsContext<Context> and requires {
        { Context::isVirtualContext(detail::IsVirtualContextTag()) } -> std::same_as<Decompress<Context>>;
    };

    struct INodeBase : Node
    {
        template<class Self, IsNodeWrapper T>
        requires IsVirtualContext<ContextOf<Self>>
        [[nodiscard]] constexpr auto exchangeImpl(this Self& self, std::in_place_type_t<T> tag, auto&&... args)
        {
            return ContextOf<Self>::exchangeImpl(std::addressof(self), tag, DI_FWD(args)...);
        }

        virtual ~INodeBase() = default;

        virtual void onGraphConstructed() {}

    private:
        template<IsInterface... Interfaces>
        friend struct di::Virtual;
        virtual void hostRelocated(void*) = 0;
    };

}

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
        template<class Trait>
        using InterfaceOf = detail::SelectIf<
            detail::NodeTraitsHasTraitPred<Trait>,
            Interfaces...
        >;

        template<class Interface>
        struct WithTrait;

        template<class Trait>
        requires (... || detail::TraitsHasTrait<typename Interfaces::Traits, Trait>)
        struct Resolver
        {
            using TraitInterface = InterfaceOf<Trait>;
            using Types = TraitInterface::Traits::template ResolveTypes<Trait>;
            using Interface = WithTrait<TraitInterface>;
        };

        template<class ImplNode>
        struct InnerContext : Context
        {
            static InnerContext isVirtualContext(detail::IsVirtualContextTag);

            template<class N, IsNodeWrapper T>
            static constexpr auto exchangeImpl(N* current, std::in_place_type_t<T> tag, auto&&... args)
            {
                Node* virtualHost = static_cast<Impl<ImplNode> const*>(current)->virtualHost;
                return virtualHost->exchangeImpl(current, tag, DI_FWD(args)...);
            }

            template<class N, IsTrait Trait>
            requires detail::HasLink<Context, Trait>
            constexpr auto getNode(N& impl, Trait trait)
            {
                Node* virtualHost = static_cast<Impl<ImplNode> const*>(std::addressof(impl))->virtualHost;
                return Context{}.getNode(std::forward_like<N&>(*virtualHost), trait);
            }
        };

        template<IsNodeWrapper T>
        using ImplOf = T::template Node<detail::CompressContext<InnerContext<T>>>;

        template<IsNodeWrapper ImplNode>
        struct Impl final : ImplOf<ImplNode>
        {
            explicit Impl(Node* virtualHost, auto&&... args)
                : Impl::Node{DI_FWD(args)...}
                , virtualHost(virtualHost)
            {}

        private:
            friend struct Node;
            Node* virtualHost;
            void hostRelocated(void* newLoc) { virtualHost = static_cast<Node*>(newLoc); };
        };

    public:
        using Traits = di::TraitsTemplate<Node, Resolver>;

        template<IsNodeWrapper T>
        requires IsInterface<Impl<T>> && (... && std::derived_from<Impl<T>, Interfaces>)
        constexpr Node(std::in_place_type_t<T>, auto&&... args)
        {
            init<T>(DI_FWD(args)...);
        }

        template<std::invocable<Constructor<Node>> F>
        requires std::same_as<Node, std::invoke_result_t<F, Constructor<Node>>>
        constexpr explicit Node(WithFactory, F factory)
            : Node(factory(Constructor<Node>()))
        {}

        template<IsNodeWrapper T>
        requires IsInterface<Impl<T>> && (... && std::derived_from<Impl<T>, Interfaces>)
        constexpr ImplOf<T>& emplace(auto&&... args)
        {
            auto const backup = interfaces;
            try
            {
                ImplOf<T>* next = init<T>(DI_FWD(args)...);
                delete get<0>(backup);
                return *next;
            }
            catch (...)
            {
                interfaces = backup;
                throw;
            }
        }

        Node(Node const&) = delete;
        constexpr Node(Node&& other)
            : interfaces(other.interfaces)
        {
            get<0>(other.interfaces) = nullptr;
            if (auto* p = get<0>(interfaces))
                p->hostRelocated(this);
        }
        constexpr ~Node()
        {
            delete get<0>(interfaces);
        }

        void onGraphConstructed()
        {
            get<0>(interfaces)->onGraphConstructed();
        }

    protected:
        template<IsNodeWrapper T>
        requires IsInterface<Impl<T>> && (... && std::derived_from<Impl<T>, Interfaces>)
        constexpr Impl<T>* init(auto&&... args)
        {
            auto p = new Impl<T>(this, DI_FWD(args)...);
            interfaces = {static_cast<Interfaces*>(p)...};
            return p;
        }

        template<class N, IsNodeWrapper T>
        constexpr auto exchangeImpl(N* current, std::in_place_type_t<T>, auto&&... args)
        {
            if (get<0>(interfaces) != current)
                throw std::runtime_error("Pointers not matching");

            auto next = init<T>(DI_FWD(args)...);
            return std::pair<std::unique_ptr<N>, ImplOf<T>&>(current, *next);
        }

        std::tuple<Interfaces*...> interfaces;
    };
};

template<IsInterface... Interfaces>
template<class Context>
template<class Interface>
struct Virtual<Interfaces...>::Node<Context>::WithTrait : Node
{
    template<class Self>
    constexpr decltype(auto) apply(this Self& self, auto&&... args)
    {
        return std::forward_like<Self&>(*get<Interface*>(self.interfaces)).apply(DI_FWD(args)...);
    }
};

}

#endif // INCLUDE_DI_VIRTUAL_HPP
