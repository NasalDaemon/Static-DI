export module di.bench.compile99_seq.node83;

import di;
export import di.bench.compile99_seq.trait.trait82;
export import di.bench.compile99_seq.trait.trait83;

namespace di::bench::compile99_seq {

export
struct Node83 : di::Node
{
    using Depends = di::Depends<trait::Trait82>;
    using Traits = di::Traits<Node83, trait::Trait83>;

    int impl(this auto const& self, trait::Trait83::get)
    {
        return self.i + self.getNode(trait::trait82).get();
    }

    Node83() = default;
    int i = 83;
};

}
