export module di.bench.compile99_seq.node32;

import di;
export import di.bench.compile99_seq.trait.trait31;
export import di.bench.compile99_seq.trait.trait32;

namespace di::bench::compile99_seq {

export
struct Node32 : di::Node
{
    using Depends = di::Depends<trait::Trait31>;
    using Traits = di::Traits<Node32, trait::Trait32>;

    int impl(this auto const& self, trait::Trait32::get)
    {
        return self.i + self.getNode(trait::trait31).get();
    }

    Node32() = default;
    int i = 32;
};

}
