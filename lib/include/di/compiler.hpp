#ifndef INCLUDE_DI_COMPILER_HPP
#define INCLUDE_DI_COMPILER_HPP

#include "di/macros.hpp"

DI_MODULE_EXPORT
namespace di::compiler {
    inline constexpr bool clang = DI_COMPILER_CLANG == 1;
    inline constexpr bool gcc = DI_COMPILER_GCC == 1;
    inline constexpr bool gnu = DI_COMPILER_GNU == 1;
    inline constexpr bool msvc = DI_COMPILER_MSVC == 1;
    inline constexpr auto version = DI_COMPILER_VER;
}

#endif // INCLUDE_DI_COMPILER_HPP
