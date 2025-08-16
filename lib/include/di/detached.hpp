#ifndef INCLUDE_DI_DETACHED_HPP
#define INCLUDE_DI_DETACHED_HPP

#include "di/detail/cast.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/context_fwd.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <type_traits>
#endif

namespace di {

struct Node;

DI_MODULE_EXPORT
struct DetachedInterface {};

DI_MODULE_EXPORT
template<class T>
concept IsDetachedInterface = std::is_base_of_v<DetachedInterface, T> and not std::is_base_of_v<Node, T>;
DI_MODULE_EXPORT
template<class T>
concept IsDetachedImpl = std::is_base_of_v<DetachedInterface, T> and std::is_base_of_v<Node, T>;

// When context is detached from the trait, then getNode and asNode calls are only available when deducing this.
// This means that the overlaid environment is also available when getNode is called.
DI_MODULE_EXPORT
template<class T>
concept HasDetachedContext = IsWrappedImpl<T> or IsDetachedImpl<T>;

DI_MODULE_EXPORT
template<class Node, IsDetachedInterface Interface>
struct DetachedImpl : Interface, private Node
{
    static_assert(std::is_empty_v<Interface>);

    DI_NODE_USE_PUBLIC_MEMBERS(Node)
    using Types = Node::Types;
    using Node::finalize;
    using Node::visit;

    // Use impl from the detached interface
    using Interface::impl;

    template<class Self>
    constexpr auto& getState(this Self& self)
    {
        ContextOf<Self>::Info::assertAccessible(self);
        return detail::upCast<Node>(self);
    }

    constexpr auto* operator->(this auto& self) { return std::addressof(self.getState()); }
};

}


#endif // INCLUDE_DI_DETACHED_HPP
