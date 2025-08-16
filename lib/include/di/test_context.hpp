#ifndef INCLUDE_DI_TEST_CONTEXT_HPP
#define INCLUDE_DI_TEST_CONTEXT_HPP

#include "di/context_fwd.hpp"

namespace di::test {

namespace detail {
struct TestContextTag{};
}

DI_MODULE_EXPORT
template<class Context>
concept IsTestContext = IsContext<Context> and requires { Context::Info::isTestContext(detail::TestContextTag{}); };

} // namespace di::test


#endif // INCLUDE_DI_TEST_CONTEXT_HPP
