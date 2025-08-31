export module di.bench.compile99_seq.node96;

import di;
export import di.bench.compile99_seq.trait.trait95;
export import di.bench.compile99_seq.trait.trait96;

namespace di::bench::compile99_seq {

export
struct Node96 : di::Node
{
    using Depends = di::Depends<trait::Trait95>;
    using Traits = di::Traits<Node96, trait::Trait96>;

    int impl(this auto const& self, trait::Trait96::get)
    {
        return self.i + self.getNode(trait::trait95).get();
    }

    Node96() = default;
    int i = 96;
};

}
