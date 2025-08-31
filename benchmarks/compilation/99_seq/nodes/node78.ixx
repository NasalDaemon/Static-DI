export module di.bench.compile99_seq.node78;

import di;
export import di.bench.compile99_seq.trait.trait77;
export import di.bench.compile99_seq.trait.trait78;

namespace di::bench::compile99_seq {

export
struct Node78 : di::Node
{
    using Depends = di::Depends<trait::Trait77>;
    using Traits = di::Traits<Node78, trait::Trait78>;

    int impl(this auto const& self, trait::Trait78::get)
    {
        return self.i + self.getNode(trait::trait77).get();
    }

    Node78() = default;
    int i = 78;
};

}
