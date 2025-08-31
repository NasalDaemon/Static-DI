export module di.bench.compile99_seq.node9;

import di;
export import di.bench.compile99_seq.trait.trait8;
export import di.bench.compile99_seq.trait.trait9;

namespace di::bench::compile99_seq {

export
struct Node9 : di::Node
{
    using Depends = di::Depends<trait::Trait8>;
    using Traits = di::Traits<Node9, trait::Trait9>;

    int impl(this auto const& self, trait::Trait9::get)
    {
        return self.i + self.getNode(trait::trait8).get();
    }

    Node9() = default;
    int i = 9;
};

}
