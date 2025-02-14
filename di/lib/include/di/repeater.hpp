#ifndef INCLUDE_DI_REPEATER_HPP
#define INCLUDE_DI_REPEATER_HPP

#include "di/detail/compress.hpp"

#include "di/context_fwd.hpp"
#include "di/environment.hpp"
#include "di/node.hpp"
#include "di/macros.hpp"
#include "di/map_info.hpp"
#include "di/trait.hpp"
#include "di/traits.hpp"

#if !DI_STD_MODULE
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

        using Types = di::ResolveTypes<Context, RepeaterTrait<0>>;

        using Traits = di::Traits<Node, Trait>;

        template<class Source, class Key>
        constexpr auto& finalize(this auto& self, Source&, Key)
        {
            using Environment = Source::Environment;
            return withEnv<Environment>(detail::downCast<WithKey<Key>>(self));
        }
    };
};

template<class Trait, std::size_t Count>
requires (Count > 0)
template<class Context>
template<class Key>
struct Repeater<Trait, Count>::Node<Context>::WithKey : Node
{
    constexpr void apply(this auto& self, auto&&... args)
    {
        apply2(std::make_index_sequence<Count>{}, self, args...);
    }

private:
    template<std::size_t... Is>
    static constexpr void apply2(std::index_sequence<Is...>, auto& repeater, auto&... args)
    {
        (apply3(repeater, Context{}.getNode(detail::upCast<Node>(repeater), RepeaterTrait<Is>{}), args...), ...);
    }

    static constexpr void apply3(auto& repeater, auto& target, auto&... args)
    {
        detail::finalize(repeater, target, Key{}).apply(args...);
    }
};

} // namespace di


#endif // INCLUDE_DI_REPEATER_HPP
