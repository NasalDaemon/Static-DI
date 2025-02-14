#if !DI_STD_MODULE
#include <cstdio>
#endif

import di.tests.thread.cluster;
import di.tests.thread.poster;
import di;
DI_IMPORT_STD;

using namespace di::tests::thread;

int test()
{
    di::Graph<Cluster> g{.a{1}, .b{2}, .c{3}};

    if (1 != di::withThread<0>(g.a)->i)
        return 1;

    if (1 != g.a.asTrait(trait::a, postTaskKey).getA())
        return 1;

    if (2 != g.a.asTrait(trait::a, postTaskKey).getB())
        return 1;

    return 0;
}

int main()
{
    int res = test();
    if (0 != res)
        std::puts("FAILED");

    return res;
}
