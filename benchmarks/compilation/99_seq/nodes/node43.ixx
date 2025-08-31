export module di.bench.compile99_seq.node43;

import di;
export import di.bench.compile99_seq.trait.trait42;
export import di.bench.compile99_seq.trait.trait43;

namespace di::bench::compile99_seq {

export
struct Node43 : di::Node
{
    using Depends = di::Depends<trait::Trait42>;
    using Traits = di::Traits<Node43, trait::Trait43>;

    int impl(this auto const& self, trait::Trait43::get)
    {
        return self.i + self.getNode(trait::trait42).get();
    }

    Node43() = default;
    int i = 43;
};

}
