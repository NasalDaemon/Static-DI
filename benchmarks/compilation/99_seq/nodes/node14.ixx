export module di.bench.compile99_seq.node14;

import di;
export import di.bench.compile99_seq.trait.trait13;
export import di.bench.compile99_seq.trait.trait14;

namespace di::bench::compile99_seq {

export
struct Node14 : di::Node
{
    using Depends = di::Depends<trait::Trait13>;
    using Traits = di::Traits<Node14, trait::Trait14>;

    int impl(this auto const& self, trait::Trait14::get)
    {
        return self.i + self.getNode(trait::trait13).get();
    }

    Node14() = default;
    int i = 14;
};

}
