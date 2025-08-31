export module di.bench.compile99_seq.node98;

import di;
export import di.bench.compile99_seq.trait.trait97;
export import di.bench.compile99_seq.trait.trait98;

namespace di::bench::compile99_seq {

export
struct Node98 : di::Node
{
    using Depends = di::Depends<trait::Trait97>;
    using Traits = di::Traits<Node98, trait::Trait98>;

    int impl(this auto const& self, trait::Trait98::get)
    {
        return self.i + self.getNode(trait::trait97).get();
    }

    Node98() = default;
    int i = 98;
};

}
