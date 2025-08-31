export module di.bench.compile99_seq.node68;

import di;
export import di.bench.compile99_seq.trait.trait67;
export import di.bench.compile99_seq.trait.trait68;

namespace di::bench::compile99_seq {

export
struct Node68 : di::Node
{
    using Depends = di::Depends<trait::Trait67>;
    using Traits = di::Traits<Node68, trait::Trait68>;

    int impl(this auto const& self, trait::Trait68::get)
    {
        return self.i + self.getNode(trait::trait67).get();
    }

    Node68() = default;
    int i = 68;
};

}
