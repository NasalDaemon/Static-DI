export module di.bench.compile99_seq.node27;

import di;
export import di.bench.compile99_seq.trait.trait26;
export import di.bench.compile99_seq.trait.trait27;

namespace di::bench::compile99_seq {

export
struct Node27 : di::Node
{
    using Depends = di::Depends<trait::Trait26>;
    using Traits = di::Traits<Node27, trait::Trait27>;

    int impl(this auto const& self, trait::Trait27::get)
    {
        return self.i + self.getNode(trait::trait26).get();
    }

    Node27() = default;
    int i = 27;
};

}
