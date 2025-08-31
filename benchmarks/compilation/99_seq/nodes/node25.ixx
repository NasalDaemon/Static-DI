export module di.bench.compile99_seq.node25;

import di;
export import di.bench.compile99_seq.trait.trait24;
export import di.bench.compile99_seq.trait.trait25;

namespace di::bench::compile99_seq {

export
struct Node25 : di::Node
{
    using Depends = di::Depends<trait::Trait24>;
    using Traits = di::Traits<Node25, trait::Trait25>;

    int impl(this auto const& self, trait::Trait25::get)
    {
        return self.i + self.getNode(trait::trait24).get();
    }

    Node25() = default;
    int i = 25;
};

}
