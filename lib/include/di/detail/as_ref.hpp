#ifndef INCLUDE_DI_DETAIL_AS_REF_HPP
#define INCLUDE_DI_DETAIL_AS_REF_HPP

#include "di/detail/compress.hpp"
#include "di/macros.hpp"
#include "di/node_fwd.hpp"
#include "di/empty_types.hpp"

#if !DI_IMPORT_STD
#include <memory>
#include <type_traits>
#endif

namespace di {

DI_MODULE_EXPORT
struct Node;
DI_MODULE_EXPORT
struct Cluster;
DI_MODULE_EXPORT
template<class ID, IsNodeHandle NodeHandle>
struct Collection;

namespace detail {
    template<IsNodeHandle MainCluster, class GlobalNodeHandle, class Root = void>
    struct GraphWithGlobalNode;
    struct ContextBase;

    // Bypass TraitView instantiation during intermediate asTrait/getNode calls
    struct AsRef
    {
    private:
        friend struct di::Node;
        friend struct di::Cluster;
        friend struct di::detail::ContextBase;
        template<class ID, IsNodeHandle NodeHandle>
        friend struct di::Collection;
        template<IsNodeHandle MainCluster, class GlobalNodeHandle, class Root>
        friend struct GraphWithGlobalNode;

        AsRef() = default;
    };

    template<class Interface_, class Types = EmptyTypes>
    struct TargetRef
    {
        using Interface = Interface_;
        explicit constexpr TargetRef(Interface& ref, std::type_identity<Types>) : ptr(std::addressof(ref)) {}
        Interface* ptr;
        static constexpr std::type_identity<CompressTypes<Types>> types() { return {}; };
        static constexpr std::type_identity<EmptyTypes> types() requires std::is_same_v<Types, EmptyTypes> { return {}; };
    };

    template<class Interface, class Types>
    TargetRef(Interface&, std::type_identity<Types>) -> TargetRef<Interface, Types>;

} // namespace detail

} // namespace di

#endif // INCLUDE_DI_DETAIL_AS_REF_HPP
