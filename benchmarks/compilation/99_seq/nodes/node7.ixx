export module di.bench.compile99_seq.node7;

import di;
export import di.bench.compile99_seq.trait.trait6;
export import di.bench.compile99_seq.trait.trait7;

namespace di::bench::compile99_seq {

export
struct Node7 : di::Node
{
    using Depends = di::Depends<trait::Trait6>;
    using Traits = di::Traits<Node7, trait::Trait7>;

    int impl(this auto const& self, trait::Trait7::get)
    {
        return self.i + self.getNode(trait::trait6).get();
    }

    Node7() = default;
    int i = 7;
};

}
