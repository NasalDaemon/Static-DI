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
    constexpr T& compressImpl(T& impl)
    {
        return impl;
    }

} // namespace di::detail

#else

template<class T>
inline constexpr auto _d1Cc_ = [] mutable
{
    struct Context : T
    {
        static T _di_uncompressedType_();
        static Context _di_compressedType_();
    };
    return std::type_identity<Context>{};
};

template<class T>
inline constexpr auto _d1Ci_ = [] mutable
{
    struct Impl : T
    {
        static T _di_uncompressedType_();
        static Impl _di_compressedType_();
    };
    return std::type_identity<Impl>{};
};

namespace di::detail {

    template<class T>
    concept IsCompressed = requires {
        T::_di_uncompressedType_();
        { T::_di_compressedType_() } -> std::same_as<T>;
    };

    template<class T>
    T decompressed();
    template<IsCompressed T>
    auto decompressed() -> decltype(T::_di_uncompressedType_());

    template<class T>
    using Decompress = decltype(decompressed<std::remove_cvref_t<T>>());

    template<class T>
    requires (not IsCompressed<T>)
    using CompressContext = decltype(_d1Cc_<T>())::type;

    template<class T>
    requires (not IsCompressed<T>)
    using CompressImpl = decltype(_d1Ci_<T>())::type;

    template<class T>
    constexpr auto& compressImpl(T& impl)
    {
        using Compressed = CompressImpl<std::remove_const_t<T>>;
        return downCast<Compressed>(impl);
    }

} // namespace di::detail

#endif

#endif // DI_DETAIL_COMPRESS_HPP
