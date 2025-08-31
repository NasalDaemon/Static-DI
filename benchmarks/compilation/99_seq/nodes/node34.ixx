export module di.bench.compile99_seq.node34;

import di;
export import di.bench.compile99_seq.trait.trait33;
export import di.bench.compile99_seq.trait.trait34;

namespace di::bench::compile99_seq {

export
struct Node34 : di::Node
{
    using Depends = di::Depends<trait::Trait33>;
    using Traits = di::Traits<Node34, trait::Trait34>;

    int impl(this auto const& self, trait::Trait34::get)
    {
        return self.i + self.getNode(trait::trait33).get();
    }

    Node34() = default;
    int i = 34;
};

}
