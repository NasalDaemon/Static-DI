export module di.bench.compile99_seq.node80;

import di;
export import di.bench.compile99_seq.trait.trait79;
export import di.bench.compile99_seq.trait.trait80;

namespace di::bench::compile99_seq {

export
struct Node80 : di::Node
{
    using Depends = di::Depends<trait::Trait79>;
    using Traits = di::Traits<Node80, trait::Trait80>;

    int impl(this auto const& self, trait::Trait80::get)
    {
        return self.i + self.getNode(trait::trait79).get();
    }

    Node80() = default;
    int i = 80;
};

}
