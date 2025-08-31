export module di.bench.compile99_seq.node41;

import di;
export import di.bench.compile99_seq.trait.trait40;
export import di.bench.compile99_seq.trait.trait41;

namespace di::bench::compile99_seq {

export
struct Node41 : di::Node
{
    using Depends = di::Depends<trait::Trait40>;
    using Traits = di::Traits<Node41, trait::Trait41>;

    int impl(this auto const& self, trait::Trait41::get)
    {
        return self.i + self.getNode(trait::trait40).get();
    }

    Node41() = default;
    int i = 41;
};

}
