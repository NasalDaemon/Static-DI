export module di.bench.compile99_seq.node97;

import di;
export import di.bench.compile99_seq.trait.trait96;
export import di.bench.compile99_seq.trait.trait97;

namespace di::bench::compile99_seq {

export
struct Node97 : di::Node
{
    using Depends = di::Depends<trait::Trait96>;
    using Traits = di::Traits<Node97, trait::Trait97>;

    int impl(this auto const& self, trait::Trait97::get)
    {
        return self.i + self.getNode(trait::trait96).get();
    }

    Node97() = default;
    int i = 97;
};

}
