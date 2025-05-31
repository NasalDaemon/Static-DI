#ifndef INCLUDE_DI_COMPILER_HPP
#define INCLUDE_DI_COMPILER_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <compare>
#endif

DI_MODULE_EXPORT
namespace di::compiler {
    enum Type
    {
        Clang, GCC, MSVC
    };

    // Opaque and strongly-typed version that can only be compared with another instance
    struct Version
    {
        struct FromMacro{};
        constexpr explicit Version(FromMacro, unsigned long long version)
            : version(version)
        {}

        constexpr explicit Version(unsigned major, unsigned minor = 0, unsigned patch = 0)
            : Version(FromMacro{}, DI_MAKE_VERSION(major, minor, patch))
        {}

        auto operator<=>(Version const&) const = default;
    private:
        unsigned long long version;
    };

    // Can only be compared to another instance
    struct Spec
    {
        constexpr Spec(Type type, Version version) : type(type), version(version) {}
        constexpr Spec(Type type, unsigned major, unsigned minor = 0, unsigned patch = 0)
            : Spec(type, Version(major, minor, patch))
        {}

        bool operator==(Spec const&) const = default;
        constexpr auto operator<=>(Spec const& other) const
        {
            return type != other.type
                ? std::partial_ordering::unordered
                : version <=> other.version;
        }

    private:
        Type type;
        Version version;
    };

    inline constexpr bool isClang = DI_COMPILER_CLANG == 1;
    inline constexpr bool isGcc = DI_COMPILER_GCC == 1;
    inline constexpr bool isGnu = DI_COMPILER_GNU == 1;
    inline constexpr bool isMsvc = DI_COMPILER_MSVC == 1;

    inline constexpr Type type = isClang ? Clang : isGcc ? GCC : MSVC;
    inline constexpr Version version(Version::FromMacro{}, DI_COMPILER_VERSION);
    inline constexpr Spec spec(type, version);

    constexpr Spec clang(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return {Clang, major, minor, patch};
    }
    constexpr Spec gcc(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return {GCC, major, minor, patch};
    }
    constexpr Spec msvc(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return {MSVC, major, minor, patch};
    }
}

#endif // INCLUDE_DI_COMPILER_HPP
