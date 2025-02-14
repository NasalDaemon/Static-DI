#if !DI_STD_MODULE
#include <cstdio>
#endif

import di;
import di.tests.repeater.cluster;
DI_IMPORT_STD;

using namespace di::tests::repeater;

int main()
{
    di::Graph<Cluster> g;
    int i = 12;
    g.a.asTrait(trait::trait).defer(i);
    if (14 != i)
    {
        std::puts("failure");
        return 1;
    }
    return 0;
}
