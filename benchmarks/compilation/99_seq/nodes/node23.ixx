export module di.bench.compile99_seq.node23;

import di;
export import di.bench.compile99_seq.trait.trait22;
export import di.bench.compile99_seq.trait.trait23;

namespace di::bench::compile99_seq {

export
struct Node23 : di::Node
{
    using Depends = di::Depends<trait::Trait22>;
    using Traits = di::Traits<Node23, trait::Trait23>;

    int impl(this auto const& self, trait::Trait23::get)
    {
        return self.i + self.getNode(trait::trait22).get();
    }

    Node23() = default;
    int i = 23;
};

}
