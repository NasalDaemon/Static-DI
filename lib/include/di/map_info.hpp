#ifndef INCLUDE_DI_NODE_MAP_INFO_HPP
#define INCLUDE_DI_NODE_MAP_INFO_HPP

#include "di/detail/concepts.hpp"
#include "di/detail/compress.hpp"

#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_STD_MODULE
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
template<class T>
concept IsInfoMapper = requires {
    typename detail::TakesUnaryClassTemplate<T::template MapInfo>;
} and std::is_empty_v<T>;

namespace detail {
    template<class Context, IsInfoMapper InfoMapper>
    struct MappedContext : Context
    {
        template<class Trait>
        requires detail::HasLink<Context, Trait>
        constexpr auto getNode(this Context context, auto& node, Trait trait)
        {
            return context.getNode(node, trait);
        }

        using Info = InfoMapper::template MapInfo<typename Context::Info>;

        static_assert(std::derived_from<Info, typename Context::Info>);
    };
}

DI_MODULE_EXPORT
template<IsNodeHandle NodeHandle, IsInfoMapper InfoMapper>
struct MapInfo
{
    template<class Context>
    using Node = ToNodeWrapper<NodeHandle>::template Node<
        detail::CompressContext<detail::MappedContext<detail::Decompress<Context>, InfoMapper>>>;
};

}

#endif // INCLUDE_DI_NODE_MAP_INFO_HPP
