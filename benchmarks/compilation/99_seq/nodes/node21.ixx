export module di.bench.compile99_seq.node21;

import di;
export import di.bench.compile99_seq.trait.trait20;
export import di.bench.compile99_seq.trait.trait21;

namespace di::bench::compile99_seq {

export
struct Node21 : di::Node
{
    using Depends = di::Depends<trait::Trait20>;
    using Traits = di::Traits<Node21, trait::Trait21>;

    int impl(this auto const& self, trait::Trait21::get)
    {
        return self.i + self.getNode(trait::trait20).get();
    }

    Node21() = default;
    int i = 21;
};

}
