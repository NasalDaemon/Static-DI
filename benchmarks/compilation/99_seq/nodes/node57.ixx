export module di.bench.compile99_seq.node57;

import di;
export import di.bench.compile99_seq.trait.trait56;
export import di.bench.compile99_seq.trait.trait57;

namespace di::bench::compile99_seq {

export
struct Node57 : di::Node
{
    using Depends = di::Depends<trait::Trait56>;
    using Traits = di::Traits<Node57, trait::Trait57>;

    int impl(this auto const& self, trait::Trait57::get)
    {
        return self.i + self.getNode(trait::trait56).get();
    }

    Node57() = default;
    int i = 57;
};

}
