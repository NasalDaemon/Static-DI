export module di.bench.compile99_seq.node8;

import di;
export import di.bench.compile99_seq.trait.trait7;
export import di.bench.compile99_seq.trait.trait8;

namespace di::bench::compile99_seq {

export
struct Node8 : di::Node
{
    using Depends = di::Depends<trait::Trait7>;
    using Traits = di::Traits<Node8, trait::Trait8>;

    int impl(this auto const& self, trait::Trait8::get)
    {
        return self.i + self.getNode(trait::trait7).get();
    }

    Node8() = default;
    int i = 8;
};

}
