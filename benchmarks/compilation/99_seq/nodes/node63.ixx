export module di.bench.compile99_seq.node63;

import di;
export import di.bench.compile99_seq.trait.trait62;
export import di.bench.compile99_seq.trait.trait63;

namespace di::bench::compile99_seq {

export
struct Node63 : di::Node
{
    using Depends = di::Depends<trait::Trait62>;
    using Traits = di::Traits<Node63, trait::Trait63>;

    int impl(this auto const& self, trait::Trait63::get)
    {
        return self.i + self.getNode(trait::trait62).get();
    }

    Node63() = default;
    int i = 63;
};

}
