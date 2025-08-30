#ifndef INCLUDE_DI_REPEATER_HPP
#define INCLUDE_DI_REPEATER_HPP

#include "di/context_fwd.hpp"
#include "di/finalise.hpp"
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

        template<std::size_t I>
        struct TypesAtT : di::ResolveTypes<Node, RepeaterTrait<I>>
        {
            static constexpr std::size_t TypesCount = Count;
            template<std::size_t Index>
            using TypesAt = TypesAtT<Index>;
        };

        using Types = TypesAtT<0>;

        template<class Source, class Key = ContextOf<Source>::Info::DefaultKey>
        DI_INLINE constexpr auto finalise(this auto& self, Source& source, Key const& key = {}, auto const&... keys)
        {
            // Don't consume the key, as it needs to be applied for each repeater trait
            return di::finalise<false>(source, self, key, keys...);
        }

        DI_INLINE constexpr void implWithKey(this auto& self, auto const& key, auto const& keys, auto&&... args)
        {
            self.applyWithKey(std::make_index_sequence<Count>{}, key, keys, args...);
        }

    private:
        template<std::size_t... Is>
        constexpr void applyWithKey(this auto& self, std::index_sequence<Is...>, auto const& key, auto const& keys, auto&... args)
        {
            (self.applyWithKey2(Context{}.getNode(detail::upCast<Node>(self), RepeaterTrait<Is>{}), key, keys, args...), ...);
        }

        constexpr void applyWithKey2(this auto& self, auto target, auto const& key, auto const& keys, auto&... args)
        {
            std::apply(
                [&](auto const&... ks)
                {
                    target.ptr->finalise(self, key, ks...)->impl(args...);
                },
                keys);
        }
    };
};

} // namespace di


#endif // INCLUDE_DI_REPEATER_HPP
