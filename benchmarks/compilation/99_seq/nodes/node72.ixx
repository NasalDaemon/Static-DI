export module di.bench.compile99_seq.node72;

import di;
export import di.bench.compile99_seq.trait.trait71;
export import di.bench.compile99_seq.trait.trait72;

namespace di::bench::compile99_seq {

export
struct Node72 : di::Node
{
    using Depends = di::Depends<trait::Trait71>;
    using Traits = di::Traits<Node72, trait::Trait72>;

    int impl(this auto const& self, trait::Trait72::get)
    {
        return self.i + self.getNode(trait::trait71).get();
    }

    Node72() = default;
    int i = 72;
};

}
