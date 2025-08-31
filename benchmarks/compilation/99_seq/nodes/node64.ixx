export module di.bench.compile99_seq.node64;

import di;
export import di.bench.compile99_seq.trait.trait63;
export import di.bench.compile99_seq.trait.trait64;

namespace di::bench::compile99_seq {

export
struct Node64 : di::Node
{
    using Depends = di::Depends<trait::Trait63>;
    using Traits = di::Traits<Node64, trait::Trait64>;

    int impl(this auto const& self, trait::Trait64::get)
    {
        return self.i + self.getNode(trait::trait63).get();
    }

    Node64() = default;
    int i = 64;
};

}
