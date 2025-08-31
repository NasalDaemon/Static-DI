export module di.bench.compile99_seq.node29;

import di;
export import di.bench.compile99_seq.trait.trait28;
export import di.bench.compile99_seq.trait.trait29;

namespace di::bench::compile99_seq {

export
struct Node29 : di::Node
{
    using Depends = di::Depends<trait::Trait28>;
    using Traits = di::Traits<Node29, trait::Trait29>;

    int impl(this auto const& self, trait::Trait29::get)
    {
        return self.i + self.getNode(trait::trait28).get();
    }

    Node29() = default;
    int i = 29;
};

}
