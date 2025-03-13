#ifndef INCLUDE_DI_DETAIL_AS_REF_HPP
#define INCLUDE_DI_DETAIL_AS_REF_HPP

#include "di/macros.hpp"

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

} // namespace detail

} // namespace di

#endif // INCLUDE_DI_DETAIL_AS_REF_HPP
