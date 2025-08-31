export module di.bench.compile99_seq.node86;

import di;
export import di.bench.compile99_seq.trait.trait85;
export import di.bench.compile99_seq.trait.trait86;

namespace di::bench::compile99_seq {

export
struct Node86 : di::Node
{
    using Depends = di::Depends<trait::Trait85>;
    using Traits = di::Traits<Node86, trait::Trait86>;

    int impl(this auto const& self, trait::Trait86::get)
    {
        return self.i + self.getNode(trait::trait85).get();
    }

    Node86() = default;
    int i = 86;
};

}
