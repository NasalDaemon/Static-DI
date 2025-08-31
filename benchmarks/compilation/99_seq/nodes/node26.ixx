export module di.bench.compile99_seq.node26;

import di;
export import di.bench.compile99_seq.trait.trait25;
export import di.bench.compile99_seq.trait.trait26;

namespace di::bench::compile99_seq {

export
struct Node26 : di::Node
{
    using Depends = di::Depends<trait::Trait25>;
    using Traits = di::Traits<Node26, trait::Trait26>;

    int impl(this auto const& self, trait::Trait26::get)
    {
        return self.i + self.getNode(trait::trait25).get();
    }

    Node26() = default;
    int i = 26;
};

}
