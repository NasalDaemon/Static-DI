export module di.bench.compile99_seq.node62;

import di;
export import di.bench.compile99_seq.trait.trait61;
export import di.bench.compile99_seq.trait.trait62;

namespace di::bench::compile99_seq {

export
struct Node62 : di::Node
{
    using Depends = di::Depends<trait::Trait61>;
    using Traits = di::Traits<Node62, trait::Trait62>;

    int impl(this auto const& self, trait::Trait62::get)
    {
        return self.i + self.getNode(trait::trait61).get();
    }

    Node62() = default;
    int i = 62;
};

}
