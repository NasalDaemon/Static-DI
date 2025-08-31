export module di.bench.compile99_seq.node30;

import di;
export import di.bench.compile99_seq.trait.trait29;
export import di.bench.compile99_seq.trait.trait30;

namespace di::bench::compile99_seq {

export
struct Node30 : di::Node
{
    using Depends = di::Depends<trait::Trait29>;
    using Traits = di::Traits<Node30, trait::Trait30>;

    int impl(this auto const& self, trait::Trait30::get)
    {
        return self.i + self.getNode(trait::trait29).get();
    }

    Node30() = default;
    int i = 30;
};

}
