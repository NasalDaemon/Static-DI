export module di.bench.compile99_seq.node85;

import di;
export import di.bench.compile99_seq.trait.trait84;
export import di.bench.compile99_seq.trait.trait85;

namespace di::bench::compile99_seq {

export
struct Node85 : di::Node
{
    using Depends = di::Depends<trait::Trait84>;
    using Traits = di::Traits<Node85, trait::Trait85>;

    int impl(this auto const& self, trait::Trait85::get)
    {
        return self.i + self.getNode(trait::trait84).get();
    }

    Node85() = default;
    int i = 85;
};

}
