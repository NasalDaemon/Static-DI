#ifndef INCLUDE_DI_DETAIL_CAST_HPP
#define INCLUDE_DI_DETAIL_CAST_HPP

#if !DI_STD_MODULE
#include <bit>
#include <memory>
#include <type_traits>
#endif

namespace di::detail {

template<class Derived, class Base>
constexpr Derived& downCast(Base& base)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    static_assert(sizeof(Derived) == sizeof(Base));
    return *(Derived*)std::addressof(base);
}

template<class Derived, class Base>
constexpr Derived const& downCast(Base const& base)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    static_assert(sizeof(Derived) == sizeof(Base));
    return *(Derived const*)std::addressof(base);
}

template<class Base, class Derived>
constexpr Base& upCast(Derived& derived)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    return *(Base*)std::addressof(derived);
}

template<class Base, class Derived>
constexpr Base const& upCast(Derived const& derived)
{
    static_assert(std::is_base_of_v<Base, Derived>);
    return *(Base const*)std::addressof(derived);
}

#if DI_COMPILER_MSVC
using MemPtrInt = std::int32_t;
#else
using MemPtrInt = std::ptrdiff_t;
#endif

template<class Class, class Member>
MemPtrInt memPtrToInt(Member Class::* memPtr)
{
    return std::bit_cast<MemPtrInt>(memPtr);
}

template<class Class, class Member>
Class Member::* reverseMemberPointer(Member Class::* memPtr)
{
    // Not constexpr, but well defined (modulo implementation)
    return std::bit_cast<Class Member::*>(-memPtrToInt(memPtr));
}

} // namespace di::detail


#endif // INCLUDE_DI_DETAIL_CAST_HPP
