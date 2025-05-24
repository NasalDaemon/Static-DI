#ifndef INCLUDE_DI_REPEATER_HPP
#define INCLUDE_DI_REPEATER_HPP

#include "di/alias.hpp"
#include "di/environment.hpp"
#include "di/node.hpp"
#include "di/macros.hpp"
#include "di/resolve.hpp"
#include "di/trait.hpp"
#include "di/traits.hpp"

#if !DI_IMPORT_STD
#include <cstddef>
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
        template<class Key>
        struct WithKey;

        template<std::size_t I>
        struct TypesAtT : di::ResolveTypes<Context, RepeaterTrait<I>>
        {
            static constexpr std::size_t TypesCount = Count;
            template<std::size_t Index>
            using TypesAt = TypesAtT<Index>;
        };

        using Types = TypesAtT<0>;

        using Traits = di::Traits<Node, Trait>;

        template<class Source, class Key>
        constexpr auto finalize(this auto& self, Source&, Key key)
        {
            using Environment = Source::Environment;
            return makeAlias(withEnv<Environment>(detail::downCast<WithKey<Key>>(self)), key);
        }
    };
};

template<class Trait, std::size_t Count>
requires (Count > 0)
template<class Context>
template<class Key>
struct Repeater<Trait, Count>::Node<Context>::WithKey : Node
{
    constexpr void applyWithKey(this auto& self, Key const& key, auto&&... args)
    {
        apply2(std::make_index_sequence<Count>{}, self, key, args...);
    }

private:
    template<std::size_t... Is>
    static constexpr void apply2(std::index_sequence<Is...>, auto& repeater, Key const& key, auto&... args)
    {
        (apply3(Context{}.getNode(detail::upCast<Node>(repeater), RepeaterTrait<Is>{}), repeater, key, args...), ...);
    }

    static constexpr void apply3(auto target, auto& repeater, Key const& key, auto&... args)
    {
        target.ptr->finalize(repeater, key)->apply(args...);
    }
};

} // namespace di


#endif // INCLUDE_DI_REPEATER_HPP
