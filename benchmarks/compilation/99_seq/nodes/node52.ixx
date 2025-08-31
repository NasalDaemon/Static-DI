export module di.bench.compile99_seq.node52;

import di;
export import di.bench.compile99_seq.trait.trait51;
export import di.bench.compile99_seq.trait.trait52;

namespace di::bench::compile99_seq {

export
struct Node52 : di::Node
{
    using Depends = di::Depends<trait::Trait51>;
    using Traits = di::Traits<Node52, trait::Trait52>;

    int impl(this auto const& self, trait::Trait52::get)
    {
        return self.i + self.getNode(trait::trait51).get();
    }

    Node52() = default;
    int i = 52;
};

}
