export module di.bench.compile99_seq.node19;

import di;
export import di.bench.compile99_seq.trait.trait18;
export import di.bench.compile99_seq.trait.trait19;

namespace di::bench::compile99_seq {

export
struct Node19 : di::Node
{
    using Depends = di::Depends<trait::Trait18>;
    using Traits = di::Traits<Node19, trait::Trait19>;

    int impl(this auto const& self, trait::Trait19::get)
    {
        return self.i + self.getNode(trait::trait18).get();
    }

    Node19() = default;
    int i = 19;
};

}
