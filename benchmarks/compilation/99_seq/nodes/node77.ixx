export module di.bench.compile99_seq.node77;

import di;
export import di.bench.compile99_seq.trait.trait76;
export import di.bench.compile99_seq.trait.trait77;

namespace di::bench::compile99_seq {

export
struct Node77 : di::Node
{
    using Depends = di::Depends<trait::Trait76>;
    using Traits = di::Traits<Node77, trait::Trait77>;

    int impl(this auto const& self, trait::Trait77::get)
    {
        return self.i + self.getNode(trait::trait76).get();
    }

    Node77() = default;
    int i = 77;
};

}
