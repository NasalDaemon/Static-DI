export module di.bench.compile99_seq.node20;

import di;
export import di.bench.compile99_seq.trait.trait19;
export import di.bench.compile99_seq.trait.trait20;

namespace di::bench::compile99_seq {

export
struct Node20 : di::Node
{
    using Depends = di::Depends<trait::Trait19>;
    using Traits = di::Traits<Node20, trait::Trait20>;

    int impl(this auto const& self, trait::Trait20::get)
    {
        return self.i + self.getNode(trait::trait19).get();
    }

    Node20() = default;
    int i = 20;
};

}
