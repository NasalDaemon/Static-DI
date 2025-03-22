#ifndef INCLUDE_DI_DETAIL_AS_REF_HPP
#define INCLUDE_DI_DETAIL_AS_REF_HPP

#include "di/detail/compress.hpp"
#include "di/macros.hpp"
#include "di/empty_types.hpp"

#if !DI_STD_MODULE
#include <memory>
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct Node;
DI_MODULE_EXPORT
struct Cluster;

namespace detail {

    struct ContextBase;

    // Bypass TraitView instantiation during intermediate asTrait/getNode calls
    struct AsRef
    {
    private:
        friend struct di::Node;
        friend struct di::Cluster;
        friend struct di::detail::ContextBase;
        AsRef() = default;
    };

    template<class Interface, class Types = EmptyTypes>
    struct TargetRef
    {
        constexpr explicit TargetRef(Interface& ref, std::type_identity<Types>) : ptr(std::addressof(ref)) {}
        Interface* ptr;
        static constexpr std::type_identity<CompressTypes<Types>> types() { return {}; };
        static constexpr std::type_identity<EmptyTypes> types() requires std::is_same_v<Types, EmptyTypes> { return {}; };
    };

    template<class Interface, class Types>
    TargetRef(Interface&, std::type_identity<Types>) -> TargetRef<Interface, Types>;

} // namespace detail

} // namespace di

#endif // INCLUDE_DI_DETAIL_AS_REF_HPP
