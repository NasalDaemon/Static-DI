export module di.bench.compile99_seq.node67;

import di;
export import di.bench.compile99_seq.trait.trait66;
export import di.bench.compile99_seq.trait.trait67;

namespace di::bench::compile99_seq {

export
struct Node67 : di::Node
{
    using Depends = di::Depends<trait::Trait66>;
    using Traits = di::Traits<Node67, trait::Trait67>;

    int impl(this auto const& self, trait::Trait67::get)
    {
        return self.i + self.getNode(trait::trait66).get();
    }

    Node67() = default;
    int i = 67;
};

}
