export module di.bench.compile99_seq.node58;

import di;
export import di.bench.compile99_seq.trait.trait57;
export import di.bench.compile99_seq.trait.trait58;

namespace di::bench::compile99_seq {

export
struct Node58 : di::Node
{
    using Depends = di::Depends<trait::Trait57>;
    using Traits = di::Traits<Node58, trait::Trait58>;

    int impl(this auto const& self, trait::Trait58::get)
    {
        return self.i + self.getNode(trait::trait57).get();
    }

    Node58() = default;
    int i = 58;
};

}
