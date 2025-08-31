export module di.bench.compile99_seq.node75;

import di;
export import di.bench.compile99_seq.trait.trait74;
export import di.bench.compile99_seq.trait.trait75;

namespace di::bench::compile99_seq {

export
struct Node75 : di::Node
{
    using Depends = di::Depends<trait::Trait74>;
    using Traits = di::Traits<Node75, trait::Trait75>;

    int impl(this auto const& self, trait::Trait75::get)
    {
        return self.i + self.getNode(trait::trait74).get();
    }

    Node75() = default;
    int i = 75;
};

}
