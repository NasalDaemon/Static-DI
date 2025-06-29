#ifndef INCLUDE_DI_REQUIRES_HPP
#define INCLUDE_DI_REQUIRES_HPP

#include "di/link.hpp"
#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/resolve.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di {

namespace detail {
    template<class Context, class Requirement, bool Transitive>
    requires HasLink<Context, Requirement>
        and (not Transitive or detail::ResolveTrait<Context, Requirement>::type::Node::Traits::template HasTrait<Requirement>)
    auto hasDependency() -> void;

    template<class Context, class Requirement, bool>
    requires std::is_pointer_v<Requirement>
    auto hasDependency() -> void;

    template<class Context, class Requirement, bool>
    auto hasDependency() -> Requirement;
}

DI_MODULE_EXPORT
template<class... Requirements>
requires (... and IsTrait<std::remove_pointer_t<Requirements>>)
struct Requires
{
    static constexpr bool isEmpty = sizeof...(Requirements) == 0;

    template<IsTrait Requirement>
    static constexpr bool contains = (... or MatchesTrait<Requirement, std::remove_pointer_t<Requirements>>);

    template<class Node, bool Transitive>
    using AssertSatisfied = detail::AllVoid<decltype(detail::hasDependency<ContextOf<Node>, Requirements, Transitive>())...>;
};

}

#endif // INCLUDE_DI_REQUIRES_HPP
