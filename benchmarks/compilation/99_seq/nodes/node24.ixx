export module di.bench.compile99_seq.node24;

import di;
export import di.bench.compile99_seq.trait.trait23;
export import di.bench.compile99_seq.trait.trait24;

namespace di::bench::compile99_seq {

export
struct Node24 : di::Node
{
    using Depends = di::Depends<trait::Trait23>;
    using Traits = di::Traits<Node24, trait::Trait24>;

    int impl(this auto const& self, trait::Trait24::get)
    {
        return self.i + self.getNode(trait::trait23).get();
    }

    Node24() = default;
    int i = 24;
};

}
