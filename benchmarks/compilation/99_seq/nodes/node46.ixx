export module di.bench.compile99_seq.node46;

import di;
export import di.bench.compile99_seq.trait.trait45;
export import di.bench.compile99_seq.trait.trait46;

namespace di::bench::compile99_seq {

export
struct Node46 : di::Node
{
    using Depends = di::Depends<trait::Trait45>;
    using Traits = di::Traits<Node46, trait::Trait46>;

    int impl(this auto const& self, trait::Trait46::get)
    {
        return self.i + self.getNode(trait::trait45).get();
    }

    Node46() = default;
    int i = 46;
};

}
