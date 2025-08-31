export module di.bench.compile99_seq.node17;

import di;
export import di.bench.compile99_seq.trait.trait16;
export import di.bench.compile99_seq.trait.trait17;

namespace di::bench::compile99_seq {

export
struct Node17 : di::Node
{
    using Depends = di::Depends<trait::Trait16>;
    using Traits = di::Traits<Node17, trait::Trait17>;

    int impl(this auto const& self, trait::Trait17::get)
    {
        return self.i + self.getNode(trait::trait16).get();
    }

    Node17() = default;
    int i = 17;
};

}
