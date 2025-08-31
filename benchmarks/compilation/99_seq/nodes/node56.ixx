export module di.bench.compile99_seq.node56;

import di;
export import di.bench.compile99_seq.trait.trait55;
export import di.bench.compile99_seq.trait.trait56;

namespace di::bench::compile99_seq {

export
struct Node56 : di::Node
{
    using Depends = di::Depends<trait::Trait55>;
    using Traits = di::Traits<Node56, trait::Trait56>;

    int impl(this auto const& self, trait::Trait56::get)
    {
        return self.i + self.getNode(trait::trait55).get();
    }

    Node56() = default;
    int i = 56;
};

}
