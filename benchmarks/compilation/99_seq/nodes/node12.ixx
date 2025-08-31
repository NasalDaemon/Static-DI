export module di.bench.compile99_seq.node12;

import di;
export import di.bench.compile99_seq.trait.trait11;
export import di.bench.compile99_seq.trait.trait12;

namespace di::bench::compile99_seq {

export
struct Node12 : di::Node
{
    using Depends = di::Depends<trait::Trait11>;
    using Traits = di::Traits<Node12, trait::Trait12>;

    int impl(this auto const& self, trait::Trait12::get)
    {
        return self.i + self.getNode(trait::trait11).get();
    }

    Node12() = default;
    int i = 12;
};

}
