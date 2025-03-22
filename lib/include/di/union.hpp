#ifndef INCLUDE_DI_UNION_HPP
#define INCLUDE_DI_UNION_HPP

#include "di/detail/cast.hpp"
#include "di/detail/type_at.hpp"
#include "di/detail/with_index.hpp"

#include "di/alias.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node.hpp"
#include "di/traits.hpp"
#include "di/trait_view.hpp"

#if !DI_STD_MODULE
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
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
        struct InnerContext_;
        using InnerContext = detail::CompressContext<InnerContext_>;
        struct InnerContext_ : detail::ContextBase
        {
            using Parent = Node;
            using ParentContext = Context;
            using Info = Context::Info;
            using Root = Context::Root;

            template<IsTrait Trait>
            static constexpr auto getNode(auto& option, Trait trait)
            {
                using Option = std::remove_cvref_t<decltype(option)>;
                static_assert((... || std::is_same_v<Option, typename ToNodeWrapper<Options>::template Node<InnerContext>>));
                auto const nodePtr = std::bit_cast<Node Option::*>(-detail::memPtrToInt(&Node::bytes));
                return Context{}.getNode(option.*nodePtr, trait);
            }
        };

    public:
        template<std::size_t I>
        using NodeAt = ToNodeWrapper<detail::TypeAt<I, Options...>>::template Node<InnerContext>;

        template<std::size_t I, class Trait>
        struct TypesAtT : NodeTypes<NodeAt<I>, Trait>
        {
            static constexpr std::size_t TypesCount = sizeof...(Options);
            template<std::size_t UnionIndex>
            using TypesAt = TypesAtT<UnionIndex, Trait>;
        };

        template<class>
        struct AsTrait;

        template<class Trait>
        requires detail::TraitsHasTrait<typename NodeAt<0>::Traits, Trait>
        using TraitsTemplate = di::ResolvedTrait<AsTrait<Trait>, TypesAtT<0, Trait>>;

        using Traits = di::TraitsTemplate<Node, TraitsTemplate>;

        constexpr explicit Node(std::size_t index)
            : index(index)
        {
            withIndex(index, [this](auto i) -> void { new (bytes) NodeAt<i>(); });
        }

        constexpr void reset(std::size_t newIndex)
        {
            destroy();
            index = newIndex;
            withIndex(index, [this](auto i) -> void { new (bytes) NodeAt<i>(); });
        }

        constexpr decltype(auto) visit(this auto& self, auto&& visitor)
        {
            return withIndex(
                self.index,
                [&](auto i) -> decltype(auto)
                {
                    return std::invoke(DI_FWD(visitor), self.template get<i>());
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

        static constexpr std::size_t Align = std::max({alignof(typename ToNodeWrapper<Options>::template Node<InnerContext>)...});
        static constexpr std::size_t Size = std::max({sizeof(typename ToNodeWrapper<Options>::template Node<InnerContext>)...});

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
    template<class Self>
    constexpr decltype(auto) applyWithKey(this Self& self, Key key, auto&&... args)
    {
        using Environment = Self::Environment;
        return self.visit([&](auto& option) -> decltype(auto)
        {
            return withEnv<Environment>(option).asTrait(Trait{}, key).apply(DI_FWD(args)...);
        });
    }
};

} // namespace di

#endif // INCLUDE_DI_UNION_HPP
