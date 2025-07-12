#ifndef INCLUDE_DI_DETAIL_CAST_HPP
#define INCLUDE_DI_DETAIL_CAST_HPP

#include "di/compiler.hpp"
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <bit>
#include <memory>
#include <type_traits>
#endif

namespace di::detail {

template<class Derived, class Base>
DI_INLINE constexpr Derived& downCast(Base& base)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    if constexpr (not isMsvc) // MSVC doesn't have proper EBO
        static_assert(sizeof(Derived) == sizeof(Base));
    return *(Derived*)std::addressof(base);
}

template<class Derived, class Base>
DI_INLINE constexpr Derived const& downCast(Base const& base)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    if constexpr (not isMsvc) // MSVC doesn't have proper EBO
        static_assert(sizeof(Derived) == sizeof(Base));
    return *(Derived const*)std::addressof(base);
}

template<class Base, class Derived>
DI_INLINE constexpr Base& upCast(Derived& derived)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    return *(Base*)std::addressof(derived);
}

template<class Base, class Derived>
DI_INLINE constexpr Base const& upCast(Derived const& derived)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    return *(Base const*)std::addressof(derived);
}

using MemPtrInt = DI_IF_MSVC_ELSE(std::int32_t)(std::ptrdiff_t);

template<class Class, class Member>
DI_INLINE MemPtrInt memPtrToInt(Member Class::* memPtr)
{
    return std::bit_cast<MemPtrInt>(memPtr);
}

template<class Class, class Member>
DI_INLINE Class Member::* reverseMemberPointer(Member Class::* memPtr)
{
    // Not constexpr, but well defined (modulo implementation)
    return std::bit_cast<Class Member::*>(-memPtrToInt(memPtr));
}

template<class Class, class Member>
Member getMemberType(Member Class::*);

} // namespace di::detail


#endif // INCLUDE_DI_DETAIL_CAST_HPP
