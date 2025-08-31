export module di.bench.compile99_seq.node40;

import di;
export import di.bench.compile99_seq.trait.trait39;
export import di.bench.compile99_seq.trait.trait40;

namespace di::bench::compile99_seq {

export
struct Node40 : di::Node
{
    using Depends = di::Depends<trait::Trait39>;
    using Traits = di::Traits<Node40, trait::Trait40>;

    int impl(this auto const& self, trait::Trait40::get)
    {
        return self.i + self.getNode(trait::trait39).get();
    }

    Node40() = default;
    int i = 40;
};

}
