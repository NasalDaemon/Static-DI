export module di.bench.compile99_seq.node28;

import di;
export import di.bench.compile99_seq.trait.trait27;
export import di.bench.compile99_seq.trait.trait28;

namespace di::bench::compile99_seq {

export
struct Node28 : di::Node
{
    using Depends = di::Depends<trait::Trait27>;
    using Traits = di::Traits<Node28, trait::Trait28>;

    int impl(this auto const& self, trait::Trait28::get)
    {
        return self.i + self.getNode(trait::trait27).get();
    }

    Node28() = default;
    int i = 28;
};

}
