export module di.bench.compile99_seq.node18;

import di;
export import di.bench.compile99_seq.trait.trait17;
export import di.bench.compile99_seq.trait.trait18;

namespace di::bench::compile99_seq {

export
struct Node18 : di::Node
{
    using Depends = di::Depends<trait::Trait17>;
    using Traits = di::Traits<Node18, trait::Trait18>;

    int impl(this auto const& self, trait::Trait18::get)
    {
        return self.i + self.getNode(trait::trait17).get();
    }

    Node18() = default;
    int i = 18;
};

}
