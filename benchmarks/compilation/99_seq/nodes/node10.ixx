export module di.bench.compile99_seq.node10;

import di;
export import di.bench.compile99_seq.trait.trait9;
export import di.bench.compile99_seq.trait.trait10;

namespace di::bench::compile99_seq {

export
struct Node10 : di::Node
{
    using Depends = di::Depends<trait::Trait9>;
    using Traits = di::Traits<Node10, trait::Trait10>;

    int impl(this auto const& self, trait::Trait10::get)
    {
        return self.i + self.getNode(trait::trait9).get();
    }

    Node10() = default;
    int i = 10;
};

}
