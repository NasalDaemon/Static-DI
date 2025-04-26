#ifndef INCLUDE_DI_LINK_HPP
#define INCLUDE_DI_LINK_HPP

#include "di/context_fwd.hpp"
#include "di/macros.hpp"
#include "di/trait.hpp"

#if !DI_IMPORT_STD
#include <concepts>
#endif

namespace di {

DI_MODULE_EXPORT
template<class T, class Trait>
concept CanGetNode = requires (T t, Trait trait) {
    { t.canGetNode(trait) } -> std::same_as<std::true_type>;
};

DI_MODULE_EXPORT
template<class T, class Trait>
concept HasTrait = requires (T t, Trait trait) {
    { t.hasTrait(trait) } -> std::same_as<std::true_type>;
};

DI_MODULE_EXPORT
template<IsContext Context_, IsTrait Trait_>
struct ResolvedLink
{
    using Context = Context_;
    using Trait = Trait_;
};

DI_MODULE_EXPORT
template<class T>
concept IsResolvedLink = requires
{
    typename T::Context;
    typename T::Trait;
};

namespace detail {

    template<class T, class Trait>
    concept HasLink = IsTrait<Trait> and requires (Trait trait) { { T::resolveLink(trait) } -> IsResolvedLink; };

    template<class T, class Trait>
    requires HasLink<T, Trait>
    using ResolveLink = decltype(T::resolveLink(std::declval<Trait>()));

    template<class T, class Trait>
    requires HasLink<T, Trait>
    using ResolveLinkContext = ResolveLink<T, Trait>::Context;

    template<class T, class Trait>
    requires HasLink<T, Trait>
    using ResolveLinkTrait = ResolveLink<T, Trait>::Trait;

    template<class T, class Trait>
    concept LinksToParent = HasLink<T, Trait> and std::is_same_v<typename T::ParentContext, ResolveLinkContext<T, Trait>>;

} // detail

}


#endif // INCLUDE_DI_LINK_HPP
