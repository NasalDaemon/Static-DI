export module di.bench.compile99_seq.node87;

import di;
export import di.bench.compile99_seq.trait.trait86;
export import di.bench.compile99_seq.trait.trait87;

namespace di::bench::compile99_seq {

export
struct Node87 : di::Node
{
    using Depends = di::Depends<trait::Trait86>;
    using Traits = di::Traits<Node87, trait::Trait87>;

    int impl(this auto const& self, trait::Trait87::get)
    {
        return self.i + self.getNode(trait::trait86).get();
    }

    Node87() = default;
    int i = 87;
};

}
