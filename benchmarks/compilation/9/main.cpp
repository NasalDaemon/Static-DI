import di.bench.compile9.graph;
import di.bench.compile9.trait.trait9;

import di;

using namespace di::bench::compile9;

// Will collapse into int main() { return 0; } in LTO release build
int main()
{
    di::Graph<Graph> g{};
    if (g->node9.asTrait(trait::trait9).get() == 45)
        return 0;
    return 1;
}
