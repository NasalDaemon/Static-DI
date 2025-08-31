export module di.bench.compile99_seq.node53;

import di;
export import di.bench.compile99_seq.trait.trait52;
export import di.bench.compile99_seq.trait.trait53;

namespace di::bench::compile99_seq {

export
struct Node53 : di::Node
{
    using Depends = di::Depends<trait::Trait52>;
    using Traits = di::Traits<Node53, trait::Trait53>;

    int impl(this auto const& self, trait::Trait53::get)
    {
        return self.i + self.getNode(trait::trait52).get();
    }

    Node53() = default;
    int i = 53;
};

}
