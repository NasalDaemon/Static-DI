export module di.bench.compile99_seq.node59;

import di;
export import di.bench.compile99_seq.trait.trait58;
export import di.bench.compile99_seq.trait.trait59;

namespace di::bench::compile99_seq {

export
struct Node59 : di::Node
{
    using Depends = di::Depends<trait::Trait58>;
    using Traits = di::Traits<Node59, trait::Trait59>;

    int impl(this auto const& self, trait::Trait59::get)
    {
        return self.i + self.getNode(trait::trait58).get();
    }

    Node59() = default;
    int i = 59;
};

}
