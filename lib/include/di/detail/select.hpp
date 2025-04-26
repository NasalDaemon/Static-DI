#ifndef INCLUDE_DI_DETAIL_ENABLE_IF_HPP
#define INCLUDE_DI_DETAIL_ENABLE_IF_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di::detail {

template<bool B>
struct EnableID
{
    template<class T>
    using ID = std::type_identity<T>;
};

template<>
struct EnableID<false>
{
    struct Disabled
    {};

    template<class T>
    using ID = Disabled;
};

template<bool B, class T>
using EnableIf = EnableID<B>::template ID<T>;

template<class... Bases>
struct InheritAll : Bases...
{};

template<class Pred, class... Ts>
using SelectIf = InheritAll<EnableIf<Pred::template value<Ts>, Ts>...>::type;

} // namespace di::detail


#endif // INCLUDE_DI_DETAIL_ENABLE_IF_HPP
