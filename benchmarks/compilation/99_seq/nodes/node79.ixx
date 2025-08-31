export module di.bench.compile99_seq.node79;

import di;
export import di.bench.compile99_seq.trait.trait78;
export import di.bench.compile99_seq.trait.trait79;

namespace di::bench::compile99_seq {

export
struct Node79 : di::Node
{
    using Depends = di::Depends<trait::Trait78>;
    using Traits = di::Traits<Node79, trait::Trait79>;

    int impl(this auto const& self, trait::Trait79::get)
    {
        return self.i + self.getNode(trait::trait78).get();
    }

    Node79() = default;
    int i = 79;
};

}
