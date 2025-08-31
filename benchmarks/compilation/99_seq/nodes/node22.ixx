export module di.bench.compile99_seq.node22;

import di;
export import di.bench.compile99_seq.trait.trait21;
export import di.bench.compile99_seq.trait.trait22;

namespace di::bench::compile99_seq {

export
struct Node22 : di::Node
{
    using Depends = di::Depends<trait::Trait21>;
    using Traits = di::Traits<Node22, trait::Trait22>;

    int impl(this auto const& self, trait::Trait22::get)
    {
        return self.i + self.getNode(trait::trait21).get();
    }

    Node22() = default;
    int i = 22;
};

}
