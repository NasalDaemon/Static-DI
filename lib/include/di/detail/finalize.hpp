#ifndef INCLUDE_DI_DETAIL_FINALIZE_HPP
#define INCLUDE_DI_DETAIL_FINALIZE_HPP

#include "di/detail/compress.hpp"

namespace di::detail {

constexpr auto& finalize(auto& source, auto& target, auto key)
{
    auto& finalized = target.finalize(source, key);
    return detail::compressImpl(finalized);
}

}

#endif // INCLUDE_DI_DETAIL_FINALIZE_HPP
