export module di.bench.compile99_seq.node88;

import di;
export import di.bench.compile99_seq.trait.trait87;
export import di.bench.compile99_seq.trait.trait88;

namespace di::bench::compile99_seq {

export
struct Node88 : di::Node
{
    using Depends = di::Depends<trait::Trait87>;
    using Traits = di::Traits<Node88, trait::Trait88>;

    int impl(this auto const& self, trait::Trait88::get)
    {
        return self.i + self.getNode(trait::trait87).get();
    }

    Node88() = default;
    int i = 88;
};

}
