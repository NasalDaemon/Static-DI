export module di.bench.compile99_seq.node2;

import di;
export import di.bench.compile99_seq.trait.trait1;
export import di.bench.compile99_seq.trait.trait2;

namespace di::bench::compile99_seq {

export
struct Node2 : di::Node
{
    using Depends = di::Depends<trait::Trait1>;
    using Traits = di::Traits<Node2, trait::Trait2>;

    int impl(this auto const& self, trait::Trait2::get)
    {
        return self.i + self.getNode(trait::trait1).get();
    }

    Node2() = default;
    int i = 2;
};

}
