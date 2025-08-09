#ifndef INCLUDE_DI_REPEATER_HPP
#define INCLUDE_DI_REPEATER_HPP

#include "di/alias.hpp"
#include "di/context_fwd.hpp"
#include "di/environment.hpp"
#include "di/node.hpp"
#include "di/macros.hpp"
#include "di/resolve.hpp"
#include "di/trait.hpp"
#include "di/traits.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
#include <utility>
#endif

namespace di {

DI_MODULE_EXPORT
template<std::size_t>
struct RepeaterTrait : di::Trait
{};

DI_MODULE_EXPORT
template<class Trait, std::size_t Count>
requires (Count > 0)
struct Repeater
{
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node, Trait>;

        template<class Key>
        struct WithKey;

        template<std::size_t I>
        struct TypesAtT : di::ResolveTypes<Node, RepeaterTrait<I>>
        {
            static constexpr std::size_t TypesCount = Count;
            template<std::size_t Index>
            using TypesAt = TypesAtT<Index>;
        };

        using Types = TypesAtT<0>;

        template<class Source, class Key = ContextOf<Source>::Info::DefaultKey>
        constexpr auto finalize(this auto& self, Source&, Key const& key = {}, auto const&... keys)
        {
            using Environment = Source::Environment;
            return makeAlias(withEnv<Environment>(detail::downCast<WithKey<Key>>(self)), key, keys...);
        }
    };
};

template<class Trait, std::size_t Count>
requires (Count > 0)
template<class Context>
template<class Key>
struct Repeater<Trait, Count>::Node<Context>::WithKey : Node
{
    template<class... Keys>
    constexpr void implWithKey(this auto& self, Key const& key, auto const& keys, auto&&... args)
    {
        apply2(std::make_index_sequence<Count>{}, self, key, keys, args...);
    }

private:
    template<std::size_t... Is>
    static constexpr void apply2(std::index_sequence<Is...>, auto& repeater, Key const& key, auto const& keys, auto&... args)
    {
        (apply3(Context{}.getNode(detail::upCast<Node>(repeater), RepeaterTrait<Is>{}), repeater, key, keys, args...), ...);
    }

    static constexpr void apply3(auto target, auto& repeater, Key const& key, auto const& keys, auto&... args)
    {
        std::apply(
            [&](auto const&... ks)
            {
                target.ptr->finalize(repeater, key, ks...)->impl(args...);
            },
            keys);
    }
};

} // namespace di


#endif // INCLUDE_DI_REPEATER_HPP
