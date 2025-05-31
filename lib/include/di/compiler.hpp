#ifndef INCLUDE_DI_COMPILER_HPP
#define INCLUDE_DI_COMPILER_HPP

#include "di/macros.hpp"

DI_MODULE_EXPORT
namespace di::compiler {
    enum Type
    {
        Clang, GCC, MSVC
    };
    inline constexpr bool clang = DI_COMPILER_CLANG == 1;
    inline constexpr bool gcc = DI_COMPILER_GCC == 1;
    inline constexpr bool gnu = DI_COMPILER_GNU == 1;
    inline constexpr bool msvc = DI_COMPILER_MSVC == 1;
    inline constexpr auto version = DI_COMPILER_VER;
    inline constexpr Type type = clang ? Clang : gcc ? GCC : MSVC;

    constexpr auto makeVersion(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return DI_MAKE_VER(major, minor, patch);
    }
    constexpr bool atLeast(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return version >= makeVersion(major, minor, patch);
    }
    constexpr bool atLeast(Type type, unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return compiler::type == type and atLeast(major, minor, patch);
    }
}

#endif // INCLUDE_DI_COMPILER_HPP
