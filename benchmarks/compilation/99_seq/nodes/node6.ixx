export module di.bench.compile99_seq.node6;

import di;
export import di.bench.compile99_seq.trait.trait5;
export import di.bench.compile99_seq.trait.trait6;

namespace di::bench::compile99_seq {

export
struct Node6 : di::Node
{
    using Depends = di::Depends<trait::Trait5>;
    using Traits = di::Traits<Node6, trait::Trait6>;

    int impl(this auto const& self, trait::Trait6::get)
    {
        return self.i + self.getNode(trait::trait5).get();
    }

    Node6() = default;
    int i = 6;
};

}
