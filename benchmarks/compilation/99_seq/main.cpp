import di.bench.compile99_seq.graph;
import di.bench.compile99_seq.trait.trait99;

import di;

using namespace di::bench::compile99_seq;

// Will collapse into int main() { return 0; } without needing LTO as there is only one translation unit
int main()
{
    di::Graph<Graph> g{};
    if (g->node99.asTrait(trait::trait99).get() == 4950)
        return 0;
    return 1;
}
