export module di.bench.compile99_seq.node54;

import di;
export import di.bench.compile99_seq.trait.trait53;
export import di.bench.compile99_seq.trait.trait54;

namespace di::bench::compile99_seq {

export
struct Node54 : di::Node
{
    using Depends = di::Depends<trait::Trait53>;
    using Traits = di::Traits<Node54, trait::Trait54>;

    int impl(this auto const& self, trait::Trait54::get)
    {
        return self.i + self.getNode(trait::trait53).get();
    }

    Node54() = default;
    int i = 54;
};

}
