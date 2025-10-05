#ifndef INCLUDE_DI_COMPILER_HPP
#define INCLUDE_DI_COMPILER_HPP

#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <compare>
#include <utility>
#endif

DI_MODULE_EXPORT
namespace di {
    // Opaque and strongly-typed version that can only be compared with another instance
    struct Version
    {
        static consteval Version fromMacro(unsigned long long version)
        {
            return Version(std::in_place, version);
        }

        consteval explicit Version(unsigned major, unsigned minor = 0, unsigned patch = 0)
            : Version(fromMacro(DI_MAKE_VERSION(major, minor, patch)))
        {
            if (patch >= 100000u)
                throw "Patch version must be < 100000";
            if (minor >= 1000u)
                throw "Minor version must be < 1000";
        }

        auto operator<=>(Version const&) const = default;

    private:
        consteval Version(std::in_place_t, unsigned long long version) : version(version) {}
        unsigned long long version;
    };

    struct Compiler
    {
        enum Kind
        {
            Clang, GCC, MSVC
        };

        bool operator==(Compiler const&) const = default;
        constexpr auto operator<=>(Compiler const& other) const
        {
            return kind != other.kind
                ? std::partial_ordering::unordered
                : version <=> other.version;
        }

        Kind kind;
        Version version;
    };

    inline constexpr bool isClang = DI_COMPILER_CLANG == 1;
    inline constexpr bool isGcc = DI_COMPILER_GCC == 1;
    inline constexpr bool isGnu = DI_COMPILER_GNU == 1;
    inline constexpr bool isMsvc = DI_COMPILER_MSVC == 1;

    inline constexpr Compiler compiler(
        isClang ? Compiler::Clang : isGcc ? Compiler::GCC : Compiler::MSVC,
        Version::fromMacro(DI_COMPILER_VERSION));

    consteval Compiler clang(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return {Compiler::Clang, Version(major, minor, patch)};
    }
    consteval Compiler gcc(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return {Compiler::GCC, Version(major, minor, patch)};
    }
    consteval Compiler msvc(unsigned major, unsigned minor = 0, unsigned patch = 0)
    {
        return {Compiler::MSVC, Version(major, minor, patch)};
    }
}

#endif // INCLUDE_DI_COMPILER_HPP
