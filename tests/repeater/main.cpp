#include <doctest/doctest.h>

#if !DI_STD_MODULE
#include <cstdio>
#endif

import di;
import di.tests.repeater.cluster;
DI_IMPORT_STD;

using namespace di::tests::repeater;

TEST_CASE("TestRepeater")
{
    di::Graph<Cluster> g;
    int i = 12;
    g.a.asTrait(trait::trait).defer(i);
    CHECK(14 == i);
}
