export module di.bench.compile99_seq.node61;

import di;
export import di.bench.compile99_seq.trait.trait60;
export import di.bench.compile99_seq.trait.trait61;

namespace di::bench::compile99_seq {

export
struct Node61 : di::Node
{
    using Depends = di::Depends<trait::Trait60>;
    using Traits = di::Traits<Node61, trait::Trait61>;

    int impl(this auto const& self, trait::Trait61::get)
    {
        return self.i + self.getNode(trait::trait60).get();
    }

    Node61() = default;
    int i = 61;
};

}
