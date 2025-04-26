#ifndef DI_DETAIL_TYPE_NAME_HPP
#define DI_DETAIL_TYPE_NAME_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <cstdint>
#include <source_location>
#include <string_view>
#include <type_traits>
#endif

namespace di::detail {

using TypeNameProber = void;

template<class T>
consteval std::string_view wrappedTypeName()
{
    return std::source_location::current().function_name();
}

consteval std::size_t wrappedTypeNamePrefixLength();
consteval std::size_t wrappedTypeNameSuffixLength();

template<class T>
consteval std::string_view typeName()
{
    if constexpr (std::is_void_v<T>)
    {
        return "void";
    }
    else
    {
        constexpr auto wrappedName = wrappedTypeName<T>();
        constexpr auto typeNameLength = wrappedName.length() - wrappedTypeNamePrefixLength() - wrappedTypeNameSuffixLength();
        return wrappedName.substr(wrappedTypeNamePrefixLength(), typeNameLength);
    };
}

consteval std::size_t wrappedTypeNamePrefixLength()
{
    return wrappedTypeName<TypeNameProber>().find(typeName<TypeNameProber>());
}

consteval std::size_t wrappedTypeNameSuffixLength()
{
    return wrappedTypeName<TypeNameProber>().length()
        - wrappedTypeNamePrefixLength()
        - typeName<TypeNameProber>().length();
}

} // namespace di::detail


#endif // DI_DETAIL_TYPE_NAME_HPP
