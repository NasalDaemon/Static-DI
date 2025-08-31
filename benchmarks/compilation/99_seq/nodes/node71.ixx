export module di.bench.compile99_seq.node71;

import di;
export import di.bench.compile99_seq.trait.trait70;
export import di.bench.compile99_seq.trait.trait71;

namespace di::bench::compile99_seq {

export
struct Node71 : di::Node
{
    using Depends = di::Depends<trait::Trait70>;
    using Traits = di::Traits<Node71, trait::Trait71>;

    int impl(this auto const& self, trait::Trait71::get)
    {
        return self.i + self.getNode(trait::trait70).get();
    }

    Node71() = default;
    int i = 71;
};

}
