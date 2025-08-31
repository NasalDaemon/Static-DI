export module di.bench.compile99_seq.node69;

import di;
export import di.bench.compile99_seq.trait.trait68;
export import di.bench.compile99_seq.trait.trait69;

namespace di::bench::compile99_seq {

export
struct Node69 : di::Node
{
    using Depends = di::Depends<trait::Trait68>;
    using Traits = di::Traits<Node69, trait::Trait69>;

    int impl(this auto const& self, trait::Trait69::get)
    {
        return self.i + self.getNode(trait::trait68).get();
    }

    Node69() = default;
    int i = 69;
};

}
