#include <doctest/doctest.h>

#if !DI_IMPORT_STD
#include <cstdio>
#endif

import di;
import di.tests.repeater.cluster;
#if DI_IMPORT_STD
import std;
#endif

using namespace di::tests::repeater;

TEST_CASE("TestRepeater")
{
    di::Graph<Cluster> g;
    int i = 12;
    g.a.asTrait(trait::trait).defer(i);
    CHECK(14 == i);
}
