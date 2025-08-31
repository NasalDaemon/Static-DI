export module di.bench.compile99_seq.node33;

import di;
export import di.bench.compile99_seq.trait.trait32;
export import di.bench.compile99_seq.trait.trait33;

namespace di::bench::compile99_seq {

export
struct Node33 : di::Node
{
    using Depends = di::Depends<trait::Trait32>;
    using Traits = di::Traits<Node33, trait::Trait33>;

    int impl(this auto const& self, trait::Trait33::get)
    {
        return self.i + self.getNode(trait::trait32).get();
    }

    Node33() = default;
    int i = 33;
};

}
