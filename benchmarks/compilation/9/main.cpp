import di.bench.compile.graph;
import di.bench.compile.trait.trait9;

import di;

using namespace di::bench::compile;

int main()
{
    di::Graph<Graph> g{};
    if (g->node9.asTrait(trait::trait9).get() == 45)
        return 0;
    return 1;
}
