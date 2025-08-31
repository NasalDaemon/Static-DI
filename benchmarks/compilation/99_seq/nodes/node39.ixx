export module di.bench.compile99_seq.node39;

import di;
export import di.bench.compile99_seq.trait.trait38;
export import di.bench.compile99_seq.trait.trait39;

namespace di::bench::compile99_seq {

export
struct Node39 : di::Node
{
    using Depends = di::Depends<trait::Trait38>;
    using Traits = di::Traits<Node39, trait::Trait39>;

    int impl(this auto const& self, trait::Trait39::get)
    {
        return self.i + self.getNode(trait::trait38).get();
    }

    Node39() = default;
    int i = 39;
};

}
