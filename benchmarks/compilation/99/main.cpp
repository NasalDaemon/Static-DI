import di.bench.compile99.graph;
import di.bench.compile99.trait.trait99;

import di;

using namespace di::bench::compile99;

// Will collapse into int main() { return 0; } in LTO release build
int main()
{
    di::Graph<Graph> g{};
    if (g->node99.asTrait(trait::trait99).get() == 4950)
        return 0;
    return 1;
}
