import di.bench.compile.graph;
import di.bench.compile.trait.trait99;

import di;

using namespace di::bench::compile;

int main()
{
    di::Graph<Graph> g{};
    if (g->node99.asTrait(trait::trait99).get() == 4950)
        return 0;
    return 1;
}
