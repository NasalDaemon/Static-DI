#ifndef INCLUDE_DI_UNION_FWD_HPP
#define INCLUDE_DI_UNION_FWD_HPP

namespace di {

namespace detail {
    struct IsUnionContextTag {};
}

DI_MODULE_EXPORT
template<class Context>
concept IsUnionContext = IsContext<Context> and requires {
    { Context::isUnionContext(detail::IsUnionContextTag()) } -> std::same_as<detail::Decompress<Context>>;
};

DI_MODULE_EXPORT
template<IsNodeHandle... Options>
struct Union;

} // namespace di

#endif // INCLUDE_DI_UNION_FWD_HPP
