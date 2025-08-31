export module di.bench.compile99_seq.node15;

import di;
export import di.bench.compile99_seq.trait.trait14;
export import di.bench.compile99_seq.trait.trait15;

namespace di::bench::compile99_seq {

export
struct Node15 : di::Node
{
    using Depends = di::Depends<trait::Trait14>;
    using Traits = di::Traits<Node15, trait::Trait15>;

    int impl(this auto const& self, trait::Trait15::get)
    {
        return self.i + self.getNode(trait::trait14).get();
    }

    Node15() = default;
    int i = 15;
};

}
