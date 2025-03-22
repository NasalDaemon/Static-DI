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

template<class T>
inline constexpr auto _d1Cc_ = []
{
    struct Context : T
    {
        static T _di_uncompressedType_();
        static Context _di_compressedType_();
    };
    return std::type_identity<Context>{};
};

template<class T>
inline constexpr auto _d1Ci_ = []
{
    struct Impl : T
    {
        static T _di_uncompressedType_();
        static Impl _di_compressedType_();
    };
    return std::type_identity<Impl>{};
};

template<class T>
inline constexpr auto _d1Ct_ = []
{
    struct Types : T
    {
        static T _di_uncompressedType_();
        static Types _di_compressedType_();
    };
    return std::type_identity<Types>{};
};

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
    using CompressContext = decltype(_d1Cc_<std::remove_cvref_t<T>>())::type;

    template<class T>
    requires (not IsCompressed<T>)
    using CompressImpl = decltype(_d1Ci_<std::remove_cvref_t<T>>())::type;

    template<class T>
    requires (not IsCompressed<T>)
    using CompressTypes = decltype(_d1Ct_<std::remove_cvref_t<T>>())::type;

    template<class T>
    requires (not IsCompressed<T>)
    constexpr auto& compressImpl(T& impl)
    {
        return downCast<CompressImpl<T>>(impl);
    }

} // namespace di::detail

#endif

#endif // DI_DETAIL_COMPRESS_HPP
