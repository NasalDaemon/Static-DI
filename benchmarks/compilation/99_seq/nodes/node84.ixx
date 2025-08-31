export module di.bench.compile99_seq.node84;

import di;
export import di.bench.compile99_seq.trait.trait83;
export import di.bench.compile99_seq.trait.trait84;

namespace di::bench::compile99_seq {

export
struct Node84 : di::Node
{
    using Depends = di::Depends<trait::Trait83>;
    using Traits = di::Traits<Node84, trait::Trait84>;

    int impl(this auto const& self, trait::Trait84::get)
    {
        return self.i + self.getNode(trait::trait83).get();
    }

    Node84() = default;
    int i = 84;
};

}
