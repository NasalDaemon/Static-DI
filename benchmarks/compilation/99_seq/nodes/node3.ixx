export module di.bench.compile99_seq.node3;

import di;
export import di.bench.compile99_seq.trait.trait2;
export import di.bench.compile99_seq.trait.trait3;

namespace di::bench::compile99_seq {

export
struct Node3 : di::Node
{
    using Depends = di::Depends<trait::Trait2>;
    using Traits = di::Traits<Node3, trait::Trait3>;

    int impl(this auto const& self, trait::Trait3::get)
    {
        return self.i + self.getNode(trait::trait2).get();
    }

    Node3() = default;
    int i = 3;
};

}
