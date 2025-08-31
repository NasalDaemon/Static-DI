export module di.bench.compile99_seq.node74;

import di;
export import di.bench.compile99_seq.trait.trait73;
export import di.bench.compile99_seq.trait.trait74;

namespace di::bench::compile99_seq {

export
struct Node74 : di::Node
{
    using Depends = di::Depends<trait::Trait73>;
    using Traits = di::Traits<Node74, trait::Trait74>;

    int impl(this auto const& self, trait::Trait74::get)
    {
        return self.i + self.getNode(trait::trait73).get();
    }

    Node74() = default;
    int i = 74;
};

}
