export module di.bench.compile99_seq.node50;

import di;
export import di.bench.compile99_seq.trait.trait49;
export import di.bench.compile99_seq.trait.trait50;

namespace di::bench::compile99_seq {

export
struct Node50 : di::Node
{
    using Depends = di::Depends<trait::Trait49>;
    using Traits = di::Traits<Node50, trait::Trait50>;

    int impl(this auto const& self, trait::Trait50::get)
    {
        return self.i + self.getNode(trait::trait49).get();
    }

    Node50() = default;
    int i = 50;
};

}
