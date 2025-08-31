export module di.bench.compile99_seq.node4;

import di;
export import di.bench.compile99_seq.trait.trait3;
export import di.bench.compile99_seq.trait.trait4;

namespace di::bench::compile99_seq {

export
struct Node4 : di::Node
{
    using Depends = di::Depends<trait::Trait3>;
    using Traits = di::Traits<Node4, trait::Trait4>;

    int impl(this auto const& self, trait::Trait4::get)
    {
        return self.i + self.getNode(trait::trait3).get();
    }

    Node4() = default;
    int i = 4;
};

}
