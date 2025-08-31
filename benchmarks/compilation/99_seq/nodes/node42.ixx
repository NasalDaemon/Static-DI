export module di.bench.compile99_seq.node42;

import di;
export import di.bench.compile99_seq.trait.trait41;
export import di.bench.compile99_seq.trait.trait42;

namespace di::bench::compile99_seq {

export
struct Node42 : di::Node
{
    using Depends = di::Depends<trait::Trait41>;
    using Traits = di::Traits<Node42, trait::Trait42>;

    int impl(this auto const& self, trait::Trait42::get)
    {
        return self.i + self.getNode(trait::trait41).get();
    }

    Node42() = default;
    int i = 42;
};

}
