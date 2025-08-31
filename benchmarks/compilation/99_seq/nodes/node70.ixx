export module di.bench.compile99_seq.node70;

import di;
export import di.bench.compile99_seq.trait.trait69;
export import di.bench.compile99_seq.trait.trait70;

namespace di::bench::compile99_seq {

export
struct Node70 : di::Node
{
    using Depends = di::Depends<trait::Trait69>;
    using Traits = di::Traits<Node70, trait::Trait70>;

    int impl(this auto const& self, trait::Trait70::get)
    {
        return self.i + self.getNode(trait::trait69).get();
    }

    Node70() = default;
    int i = 70;
};

}
