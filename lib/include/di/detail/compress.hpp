#ifndef DI_DETAIL_COMPRESS_HPP
#define DI_DETAIL_COMPRESS_HPP

#include "di/detail/cast.hpp"

#if DI_DECOMPRESS_TYPES

namespace di::detail {

    template<class T>
    concept IsCompressed = false;

    template<class T>
    using Decompress = std::remove_cvref_t<T>;

    template<class T>
    using CompressContext = std::remove_cvref_t<T>;

    template<class T>
    using CompressImpl = std::remove_cvref_t<T>;

    template<class T>
    using CompressTypes = std::remove_cvref_t<T>;

    template<class T>
    constexpr T& compressImpl(T& impl)
    {
        return impl;
    }

} // namespace di::detail

#else

#define DI_COMPRESSOR_UGLY(id) _d1C##id##_

#define DI_DEFINE_COMPRESSOR(id, Name) \
    constexpr auto DI_COMPRESSOR_UGLY(id)() \
    { \
        return []<class T>() \
        { \
            struct Name : T \
            { \
                static T _di_uncompressedType_(); \
                static Name _di_compressedType_(); \
            }; \
            return std::type_identity<Name>{}; \
        }; \
    }

#define DI_CALL_COMPRESSOR(id, T) \
    ::DI_COMPRESSOR_UGLY(id)().template operator()<std::remove_cvref_t<T>>()

DI_DEFINE_COMPRESSOR(c, Context)
DI_DEFINE_COMPRESSOR(i, Impl)
DI_DEFINE_COMPRESSOR(t, Types)

namespace di::detail {

    template<class T>
    concept IsCompressed = requires(T t) {
        t._di_uncompressedType_();
        { t._di_compressedType_() } -> std::same_as<std::remove_cvref_t<T>>;
    };

    template<class T>
    T decompressed();
    template<IsCompressed T>
    auto decompressed() -> decltype(T::_di_uncompressedType_());

    template<class T>
    using Decompress = decltype(decompressed<std::remove_cvref_t<T>>());

    template<class T>
    requires (not IsCompressed<T>)
    using CompressContext = decltype(DI_CALL_COMPRESSOR(c, T))::type;

    template<class T>
    requires (not IsCompressed<T>)
    using CompressImpl = decltype(DI_CALL_COMPRESSOR(i, T))::type;

    template<class T>
    requires (not IsCompressed<T>)
    using CompressTypes = decltype(DI_CALL_COMPRESSOR(t, T))::type;

    template<class T>
    requires (not IsCompressed<T>)
    constexpr auto& compressImpl(T& impl)
    {
        return downCast<CompressImpl<T>>(impl);
    }

} // namespace di::detail

#endif

#endif // DI_DETAIL_COMPRESS_HPP
