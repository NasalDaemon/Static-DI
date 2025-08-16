#ifndef INCLUDE_DI_MOCK_FWD_HPP
#define INCLUDE_DI_MOCK_FWD_HPP

#include "di/empty_types.hpp"

namespace di::test {

DI_MODULE_EXPORT
template<class DefaultTypes = EmptyTypes, class... MockedTraits>
struct Mock;

}

#endif // INCLUDE_DI_MOCK_FWD_HPP
