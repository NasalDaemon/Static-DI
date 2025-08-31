export module di.bench.compile99_seq.node81;

import di;
export import di.bench.compile99_seq.trait.trait80;
export import di.bench.compile99_seq.trait.trait81;

namespace di::bench::compile99_seq {

export
struct Node81 : di::Node
{
    using Depends = di::Depends<trait::Trait80>;
    using Traits = di::Traits<Node81, trait::Trait81>;

    int impl(this auto const& self, trait::Trait81::get)
    {
        return self.i + self.getNode(trait::trait80).get();
    }

    Node81() = default;
    int i = 81;
};

}
