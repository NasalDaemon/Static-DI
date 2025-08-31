export module di.bench.compile99_seq.node65;

import di;
export import di.bench.compile99_seq.trait.trait64;
export import di.bench.compile99_seq.trait.trait65;

namespace di::bench::compile99_seq {

export
struct Node65 : di::Node
{
    using Depends = di::Depends<trait::Trait64>;
    using Traits = di::Traits<Node65, trait::Trait65>;

    int impl(this auto const& self, trait::Trait65::get)
    {
        return self.i + self.getNode(trait::trait64).get();
    }

    Node65() = default;
    int i = 65;
};

}
