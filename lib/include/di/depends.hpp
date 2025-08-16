#ifndef INCLUDE_DI_REQUIRES_HPP
#define INCLUDE_DI_REQUIRES_HPP

#include "di/global_context.hpp"
#include "di/global_trait.hpp"
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
        and (not Transitive or detail::ResolveTrait<Context, Requirement>::Node::Traits::template HasTrait<Requirement>)
    auto dependencySatisfied() -> void;

    template<class Context, IsGlobalTrait Requirement, bool Transitive>
    requires ContextHasGlobalTrait<Context, Requirement>
        and (not Transitive or detail::ResolveTrait<Context, Requirement>::Node::Traits::template HasTrait<typename Requirement::Trait>)
    auto dependencySatisfied() -> void;

    // When dependency is a pointer, it is optional and not to be enforced
    template<class Context, class Requirement, bool>
    requires std::is_pointer_v<Requirement>
    auto dependencySatisfied() -> void;

    // On failure, return the requirement type for better error messages
    template<class Context, class Requirement, bool>
    auto dependencySatisfied() -> Requirement;

    struct DependsImplicitly
    {
        static constexpr bool isSpecified = false;

        // When no dependencies are specified, all dependencies are implicit and therefore allowed
        template<IsTrait>
        static constexpr bool dependencyAllowed = true;

        template<class, bool>
        using AssertSatisfied = void;
    };
}

DI_MODULE_EXPORT
template<class... Traits>
requires (... and IsTrait<std::remove_pointer_t<Traits>>)
struct Depends
{
    static constexpr bool isSpecified = true;

    // When dependencies are specified, all dependencies must be listed explicitly
    template<IsTrait Trait>
    static constexpr bool dependencyAllowed = (... or MatchesTrait<Trait, std::remove_pointer_t<Traits>>);

    // On failure, the missing required trait types are named in a list for better error messages
    template<class Node, bool Transitive>
    using AssertSatisfied = detail::AllVoid<decltype(detail::dependencySatisfied<ContextOf<Node>, Traits, Transitive>())...>;
};

} // namespace di

#endif // INCLUDE_DI_REQUIRES_HPP
