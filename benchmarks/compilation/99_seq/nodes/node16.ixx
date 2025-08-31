export module di.bench.compile99_seq.node16;

import di;
export import di.bench.compile99_seq.trait.trait15;
export import di.bench.compile99_seq.trait.trait16;

namespace di::bench::compile99_seq {

export
struct Node16 : di::Node
{
    using Depends = di::Depends<trait::Trait15>;
    using Traits = di::Traits<Node16, trait::Trait16>;

    int impl(this auto const& self, trait::Trait16::get)
    {
        return self.i + self.getNode(trait::trait15).get();
    }

    Node16() = default;
    int i = 16;
};

}
