export module di.bench.compile99_seq.node95;

import di;
export import di.bench.compile99_seq.trait.trait94;
export import di.bench.compile99_seq.trait.trait95;

namespace di::bench::compile99_seq {

export
struct Node95 : di::Node
{
    using Depends = di::Depends<trait::Trait94>;
    using Traits = di::Traits<Node95, trait::Trait95>;

    int impl(this auto const& self, trait::Trait95::get)
    {
        return self.i + self.getNode(trait::trait94).get();
    }

    Node95() = default;
    int i = 95;
};

}
