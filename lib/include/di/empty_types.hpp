#ifndef INCLUDE_DI_EMPTY_TYPES_HPP
#define INCLUDE_DI_EMPTY_TYPES_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct EmptyTypes{};

DI_MODULE_EXPORT
template<class T>
concept IsStateless = std::is_empty_v<T>;

} // namespace di

#endif // INCLUDE_DI_EMPTY_TYPES_HPP
