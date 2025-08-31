export module di.bench.compile99_seq.node37;

import di;
export import di.bench.compile99_seq.trait.trait36;
export import di.bench.compile99_seq.trait.trait37;

namespace di::bench::compile99_seq {

export
struct Node37 : di::Node
{
    using Depends = di::Depends<trait::Trait36>;
    using Traits = di::Traits<Node37, trait::Trait37>;

    int impl(this auto const& self, trait::Trait37::get)
    {
        return self.i + self.getNode(trait::trait36).get();
    }

    Node37() = default;
    int i = 37;
};

}
