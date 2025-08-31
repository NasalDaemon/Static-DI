export module di.bench.compile99_seq.node11;

import di;
export import di.bench.compile99_seq.trait.trait10;
export import di.bench.compile99_seq.trait.trait11;

namespace di::bench::compile99_seq {

export
struct Node11 : di::Node
{
    using Depends = di::Depends<trait::Trait10>;
    using Traits = di::Traits<Node11, trait::Trait11>;

    int impl(this auto const& self, trait::Trait11::get)
    {
        return self.i + self.getNode(trait::trait10).get();
    }

    Node11() = default;
    int i = 11;
};

}
