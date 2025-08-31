export module di.bench.compile99_seq.node5;

import di;
export import di.bench.compile99_seq.trait.trait4;
export import di.bench.compile99_seq.trait.trait5;

namespace di::bench::compile99_seq {

export
struct Node5 : di::Node
{
    using Depends = di::Depends<trait::Trait4>;
    using Traits = di::Traits<Node5, trait::Trait5>;

    int impl(this auto const& self, trait::Trait5::get)
    {
        return self.i + self.getNode(trait::trait4).get();
    }

    Node5() = default;
    int i = 5;
};

}
