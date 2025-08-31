export module di.bench.compile99_seq.node91;

import di;
export import di.bench.compile99_seq.trait.trait90;
export import di.bench.compile99_seq.trait.trait91;

namespace di::bench::compile99_seq {

export
struct Node91 : di::Node
{
    using Depends = di::Depends<trait::Trait90>;
    using Traits = di::Traits<Node91, trait::Trait91>;

    int impl(this auto const& self, trait::Trait91::get)
    {
        return self.i + self.getNode(trait::trait90).get();
    }

    Node91() = default;
    int i = 91;
};

}
