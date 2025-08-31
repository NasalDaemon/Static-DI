export module di.bench.compile99_seq.node92;

import di;
export import di.bench.compile99_seq.trait.trait91;
export import di.bench.compile99_seq.trait.trait92;

namespace di::bench::compile99_seq {

export
struct Node92 : di::Node
{
    using Depends = di::Depends<trait::Trait91>;
    using Traits = di::Traits<Node92, trait::Trait92>;

    int impl(this auto const& self, trait::Trait92::get)
    {
        return self.i + self.getNode(trait::trait91).get();
    }

    Node92() = default;
    int i = 92;
};

}
