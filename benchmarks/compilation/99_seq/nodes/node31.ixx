export module di.bench.compile99_seq.node31;

import di;
export import di.bench.compile99_seq.trait.trait30;
export import di.bench.compile99_seq.trait.trait31;

namespace di::bench::compile99_seq {

export
struct Node31 : di::Node
{
    using Depends = di::Depends<trait::Trait30>;
    using Traits = di::Traits<Node31, trait::Trait31>;

    int impl(this auto const& self, trait::Trait31::get)
    {
        return self.i + self.getNode(trait::trait30).get();
    }

    Node31() = default;
    int i = 31;
};

}
