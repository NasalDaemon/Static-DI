#include <doctest/doctest.h>

#if !DI_IMPORT_STD
#include <cstdio>
#endif

import di.tests.repeater.cluster;
import di;

namespace di::tests::repeater {

TEST_CASE("TestRepeater")
{
    di::Graph<Cluster> g;
    int i = 12;
    g.a.asTrait(trait::trait).defer(i);
    CHECK(14 == i);
}

} // namespace di::tests::repeater
