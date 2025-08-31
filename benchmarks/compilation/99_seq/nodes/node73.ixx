export module di.bench.compile99_seq.node73;

import di;
export import di.bench.compile99_seq.trait.trait72;
export import di.bench.compile99_seq.trait.trait73;

namespace di::bench::compile99_seq {

export
struct Node73 : di::Node
{
    using Depends = di::Depends<trait::Trait72>;
    using Traits = di::Traits<Node73, trait::Trait73>;

    int impl(this auto const& self, trait::Trait73::get)
    {
        return self.i + self.getNode(trait::trait72).get();
    }

    Node73() = default;
    int i = 73;
};

}
