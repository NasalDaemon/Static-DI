export module di.bench.compile99_seq.node82;

import di;
export import di.bench.compile99_seq.trait.trait81;
export import di.bench.compile99_seq.trait.trait82;

namespace di::bench::compile99_seq {

export
struct Node82 : di::Node
{
    using Depends = di::Depends<trait::Trait81>;
    using Traits = di::Traits<Node82, trait::Trait82>;

    int impl(this auto const& self, trait::Trait82::get)
    {
        return self.i + self.getNode(trait::trait81).get();
    }

    Node82() = default;
    int i = 82;
};

}
