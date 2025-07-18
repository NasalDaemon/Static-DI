#ifndef INCLUDE_DI_UNION_HPP
#define INCLUDE_DI_UNION_HPP

#include "di/detail/cast.hpp"
#include "di/detail/concepts.hpp"
#include "di/detail/select.hpp"
#include "di/detail/type_at.hpp"
#include "di/detail/with_index.hpp"

#include "di/alias.hpp"
#include "di/context_fwd.hpp"
#include "di/factory.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"
#include "di/trait_view.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
template<IsNodeHandle... Options>
struct Union
{
    static constexpr auto withIndex = di::withIndex<sizeof...(Options)>;

    template<class Context>
    class Node : public di::Node
    {
        struct InnerContext : Context
        {
            template<class Option, IsTrait Trait>
            requires detail::HasLink<Context, Trait>
            static constexpr auto getNode(Option& option, Trait trait)
            {
                using OptionNode = detail::SelectIf<
                    detail::NodeHasTraitsNodePred<typename Option::Traits::Node>,
                    ToNode<Options>...
                >;
                auto const nodePtr = std::bit_cast<Node OptionNode::*>(-detail::memPtrToInt(&Node::bytes));
                return Context{}.getNode(detail::downCast<OptionNode>(option).*nodePtr, trait);
            }

            template<class Self>
            void getPeerMemPtr(this Self)
            {
                static_assert(detail::alwaysFalse<Self>, "May not access peers from within di::Union");
            }
        };

        template<class Option>
        using ToNode = detail::ToNodeState<typename ToNodeWrapper<Option>::template Node<detail::CompressContext<InnerContext>>>;

    public:
        template<std::size_t I>
        using NodeAt = ToNode<detail::TypeAt<I, Options...>>;

        static constexpr bool isUnary() { return (... and ToNode<Options>::isUnary()); }

        template<std::size_t I, class Trait>
        struct TypesAtT : detail::ResolveTrait<NodeAt<I>, Trait>::type::Types
        {
            static constexpr std::size_t TypesCount = sizeof...(Options);
            template<std::size_t UnionIndex>
            using TypesAt = TypesAtT<UnionIndex, Trait>;
        };

        template<class>
        struct AsTrait;

        template<class Trait>
        requires HasTrait<NodeAt<0>, Trait>
        using TraitsTemplate = di::ResolvedTrait<AsTrait<Trait>, TypesAtT<0, Trait>>;

        using Traits = di::TraitsTemplate<Node, TraitsTemplate>;

        template<std::invocable<Constructor<Node>> F>
        requires std::same_as<Node, std::invoke_result_t<F, Constructor<Node>>>
        explicit constexpr Node(WithFactory, F factory)
            : Node(factory(Constructor<Node>()))
        {}

        template<std::size_t I>
        requires (I < sizeof...(Options))
        explicit(false) constexpr Node(std::in_place_index_t<I>, auto&&... args)
            : index(I)
        {
            new (bytes) NodeAt<I>{DI_FWD(args)...};
        }

        template<std::size_t I>
        requires (I < sizeof...(Options))
        constexpr NodeAt<I>& emplace(auto&&... args)
        {
            destroy();
            index = I;
            return *new (bytes) NodeAt<I>{DI_FWD(args)...};
        }

        template<class Option>
        requires (... || std::same_as<Option, Options>)
        explicit(false) constexpr Node(std::in_place_type_t<Option>, auto&&... args)
            : index(findIndex<Option>())
        {
            new (bytes) ToNode<Option>{DI_FWD(args)...};
        }

        template<class Option>
        requires (... || std::same_as<Option, Options>)
        constexpr ToNode<Option>& emplace(auto&&... args)
        {
            destroy();
            index = findIndex<Option>();
            return *new (bytes) ToNode<Option>{DI_FWD(args)...};
        }

        template<class Visitor>
        constexpr decltype(auto) visit(this auto& self, Visitor&& visitor)
        {
            return withIndex(
                self.index,
                [&](auto i) -> decltype(auto)
                {
                    return std::invoke(DI_FWD(Visitor, visitor), self.template get<i>());
                });
        }

        constexpr ~Node()
        {
            destroy();
        }

        template<std::size_t Index>
        constexpr auto& get()
        {
            return *std::launder(reinterpret_cast<NodeAt<Index>*>(bytes));
        }

        template<std::size_t Index>
        constexpr auto const& get() const
        {
            return *std::launder(reinterpret_cast<NodeAt<Index> const*>(bytes));
        }

    private:
        constexpr void destroy()
        {
            visit([]<class T>(T& impl) -> void { impl.~T(); });
        }

        template<class Option>
        static consteval std::size_t findIndex()
        {
            std::size_t i = 0, c = 0;
            (((i |= std::is_same_v<Option, Options>, (c += i))), ...);
            if (c == 0)
                throw "Not a valid Option";
            return sizeof...(Options) - c;
        }

        static constexpr std::size_t Align = std::max({alignof(ToNode<Options>)...});
        static constexpr std::size_t Size = std::max({sizeof(ToNode<Options>)...});

        std::size_t index;
        alignas(Align) std::byte bytes[Size];
    };
};

template<IsNodeHandle... Options>
template<class Context>
template<class Trait>
struct Union<Options...>::Node<Context>::AsTrait : Node
{
    template<class Key>
    struct WithKey;

    template<class Source, class Key>
    constexpr auto finalize(this auto& self, Source&, Key key)
    {
        using Environment = Source::Environment;
        return makeAlias(withEnv<Environment>(detail::downCast<WithKey<Key>>(self)), key);
    }
};

template<IsNodeHandle... Options>
template<class Context>
template<class Trait>
template<class Key>
struct Union<Options...>::Node<Context>::AsTrait<Trait>::WithKey : AsTrait
{
    template<class Self, class... Args>
    constexpr decltype(auto) implWithKey(this Self& self, Key key, Args&&... args)
    {
        using Environment = Self::Environment;
        return self.visit([&](auto& option) -> decltype(auto)
        {
            return withEnv<Environment>(option).asTrait(Trait{}, key).impl(DI_FWD(Args, args)...);
        });
    }
};

} // namespace di

#endif // INCLUDE_DI_UNION_HPP
