export module di.bench.compile99_seq.node13;

import di;
export import di.bench.compile99_seq.trait.trait12;
export import di.bench.compile99_seq.trait.trait13;

namespace di::bench::compile99_seq {

export
struct Node13 : di::Node
{
    using Depends = di::Depends<trait::Trait12>;
    using Traits = di::Traits<Node13, trait::Trait13>;

    int impl(this auto const& self, trait::Trait13::get)
    {
        return self.i + self.getNode(trait::trait12).get();
    }

    Node13() = default;
    int i = 13;
};

}
