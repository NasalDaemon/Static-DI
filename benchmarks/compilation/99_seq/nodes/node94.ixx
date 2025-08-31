export module di.bench.compile99_seq.node94;

import di;
export import di.bench.compile99_seq.trait.trait93;
export import di.bench.compile99_seq.trait.trait94;

namespace di::bench::compile99_seq {

export
struct Node94 : di::Node
{
    using Depends = di::Depends<trait::Trait93>;
    using Traits = di::Traits<Node94, trait::Trait94>;

    int impl(this auto const& self, trait::Trait94::get)
    {
        return self.i + self.getNode(trait::trait93).get();
    }

    Node94() = default;
    int i = 94;
};

}
