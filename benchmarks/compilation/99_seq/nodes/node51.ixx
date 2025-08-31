export module di.bench.compile99_seq.node51;

import di;
export import di.bench.compile99_seq.trait.trait50;
export import di.bench.compile99_seq.trait.trait51;

namespace di::bench::compile99_seq {

export
struct Node51 : di::Node
{
    using Depends = di::Depends<trait::Trait50>;
    using Traits = di::Traits<Node51, trait::Trait51>;

    int impl(this auto const& self, trait::Trait51::get)
    {
        return self.i + self.getNode(trait::trait50).get();
    }

    Node51() = default;
    int i = 51;
};

}
