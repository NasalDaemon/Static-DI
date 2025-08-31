export module di.bench.compile99_seq.node66;

import di;
export import di.bench.compile99_seq.trait.trait65;
export import di.bench.compile99_seq.trait.trait66;

namespace di::bench::compile99_seq {

export
struct Node66 : di::Node
{
    using Depends = di::Depends<trait::Trait65>;
    using Traits = di::Traits<Node66, trait::Trait66>;

    int impl(this auto const& self, trait::Trait66::get)
    {
        return self.i + self.getNode(trait::trait65).get();
    }

    Node66() = default;
    int i = 66;
};

}
