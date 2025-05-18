#include <doctest/doctest.h>

#if !DI_IMPORT_STD
#include <cstdio>
#endif

import di.tests.thread.cluster;
import di.tests.thread.poster;
import di;
#if DI_IMPORT_STD
import std;
#endif

namespace di::tests::thread {

TEST_CASE("TestThread")
{
    di::Graph<Cluster> g{.a{1}, .b{2}, .c{3}};

    CHECK(1 == di::withThread<0>(g.a)->i);

    CHECK(1 == g.a.asTrait(trait::a, postTaskKey).getA());

    CHECK(2 == g.a.asTrait(trait::a, postTaskKey).getB());
}

} // namespace di::tests::thread
