export module di.bench.compile99_seq.node45;

import di;
export import di.bench.compile99_seq.trait.trait44;
export import di.bench.compile99_seq.trait.trait45;

namespace di::bench::compile99_seq {

export
struct Node45 : di::Node
{
    using Depends = di::Depends<trait::Trait44>;
    using Traits = di::Traits<Node45, trait::Trait45>;

    int impl(this auto const& self, trait::Trait45::get)
    {
        return self.i + self.getNode(trait::trait44).get();
    }

    Node45() = default;
    int i = 45;
};

}
