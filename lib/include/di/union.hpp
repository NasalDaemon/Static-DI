#ifndef INCLUDE_DI_UNION_HPP
#define INCLUDE_DI_UNION_HPP

#include "di/detail/cast.hpp"
#include "di/detail/concepts.hpp"
#include "di/detail/select.hpp"
#include "di/detail/type_at.hpp"
#include "di/detail/with_index.hpp"

#include "di/alias.hpp"
#include "di/context_fwd.hpp"
#include "di/defer.hpp"
#include "di/factory.hpp"
#include "di/finalise.hpp"
#include "di/link.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/resolve.hpp"
#include "di/traits.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>
#endif

namespace di {

namespace detail {
    struct IsUnionContextTag {};
}

DI_MODULE_EXPORT
template<class Context>
concept IsUnionContext = IsContext<Context> and requires {
    { Context::isUnionContext(detail::IsUnionContextTag()) } -> std::same_as<detail::Decompress<Context>>;
};

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
            static InnerContext isUnionContext(detail::IsUnionContextTag);

            template<IsNodeHandle T, class Current>
            requires (not std::is_const_v<Current>)
            static constexpr auto exchangeImpl(Current& current, auto&&... args)
            {
                static_assert((... or std::is_same_v<T, Options>), "Cannot exchange with a node that is not listed as an option in the hosting di::Union");
                auto const nodePtr = di::detail::memberPtr<Node>(std::bit_cast<Current Node::*>(&Node::bytes));
                Node& node = nodePtr.getClassFromMember(current);
                // Take args by value in case they come from the current node's state, which will be invalidated during the call to emplace
                return di::Defer(
                    [&node, ...args = DI_FWD(args)]() mutable
                    {
                        node.template emplace<T>(std::move(args)...);
                    });
            }

            template<class Option, IsTrait Trait>
            requires detail::HasLink<Context, Trait>
            static constexpr auto getNode(Option& option, Trait trait)
            {
                using OptionNode = detail::SelectIf<
                    detail::NodeHasTraitsNodePred<typename Option::Traits::Node>,
                    ToNode<Options>...
                >;
                auto const nodePtr = detail::memberPtr<Node>(std::bit_cast<OptionNode Node::*>(&Node::bytes));
                return Context{}.getNode(nodePtr.getClassFromMember(option), trait);
            }

            template<IsContext Parent>
            constexpr auto getParentMemPtr()
            {
                // Disable member pointer access to children as the implementation can be swapped out at runtime
                static_assert(detail::alwaysFalse<Parent>, "di::Union does not have a stable member pointer relative to its parent node");
            }

            template<IsContext Parent, class Option>
            constexpr auto& getParentNode(Option& node)
            {
                if constexpr (std::is_same_v<detail::Decompress<Parent>, InnerContext>)
                {
                    return node;
                }
                else
                {
                    using OptionNode = detail::SelectIf<
                        detail::NodeHasTraitsNodePred<typename Option::Traits::Node>,
                        ToNode<Options>...
                    >;
                    auto const nodePtr = detail::memberPtr<Node>(std::bit_cast<OptionNode Node::*>(&Node::bytes));
                    return Context{}.template getParentNode<Parent>(nodePtr.getClassFromMember(node));
                }
            }
        };

        template<class Option>
        using ToNode = detail::ToNodeState<typename ToNodeWrapper<Option>::template Node<detail::CompressContext<InnerContext>>>;

    public:
        template<std::size_t I>
        using NodeAt = ToNode<detail::TypeAt<I, Options...>>;

        static constexpr bool isUnary() { return (... and ToNode<Options>::isUnary()); }

        template<std::size_t I, class Trait>
        struct TypesAtT : detail::ResolveTrait<NodeAt<I>, Trait>::Types
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
        constexpr auto& get() &
        {
            return *std::launder(reinterpret_cast<NodeAt<Index>*>(bytes));
        }

        template<std::size_t Index>
        constexpr auto const& get() const &
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
    template<class Source, class Key = ContextOf<Source>::Info::DefaultKey>
    constexpr auto finalise(this auto& self, Source& source, Key const& key = {}, auto const&... keys)
    {
        // Don't consume the key, as it needs to be applied once we know the active option
        return di::finalise<false>(source, self, key, keys...);
    }

    template<class Self, class... Args>
    constexpr decltype(auto) implWithKey(this Self& self, auto const& key, auto const& keys, Args&&... args)
    {
        using Environment = Self::Environment;
        return self.visit(
            [&](auto& option) -> decltype(auto)
            {
                return std::apply(
                    [&](auto const&... ks) -> decltype(auto)
                    {
                        return withEnv<Environment>(option).asTrait(Trait{}, key, ks...).impl(DI_FWD(Args, args)...);
                    },
                    keys);
            });
    }
};

} // namespace di

#endif // INCLUDE_DI_UNION_HPP
