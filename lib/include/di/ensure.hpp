#ifndef INCLUDE_DI_ENSURE_HPP
#define INCLUDE_DI_ENSURE_HPP

#include "di/count.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
template<NodeDependenciesSatisfied T, template<class> class... Pred>
requires (... and Pred<T>::value)
using Ensure = T;

namespace pred {

    DI_MODULE_EXPORT
    template<IsUnary Node>
    using Unary = std::true_type;

    DI_MODULE_EXPORT
    template<class Node>
    requires (not IsUnary<Node>)
    using NonUnary = std::true_type;

    DI_MODULE_EXPORT
    template<class Node>
    requires std::is_empty_v<Node>
    using Stateless = std::true_type;

    DI_MODULE_EXPORT
    template<NodeHasDepends Node>
    using HasDepends = std::true_type;

}

}

#endif // INCLUDE_DI_ENSURE_HPP
