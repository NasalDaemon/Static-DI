export module di.bench.compile99_seq.node89;

import di;
export import di.bench.compile99_seq.trait.trait88;
export import di.bench.compile99_seq.trait.trait89;

namespace di::bench::compile99_seq {

export
struct Node89 : di::Node
{
    using Depends = di::Depends<trait::Trait88>;
    using Traits = di::Traits<Node89, trait::Trait89>;

    int impl(this auto const& self, trait::Trait89::get)
    {
        return self.i + self.getNode(trait::trait88).get();
    }

    Node89() = default;
    int i = 89;
};

}
