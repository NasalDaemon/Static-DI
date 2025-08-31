export module di.bench.compile99_seq.node36;

import di;
export import di.bench.compile99_seq.trait.trait35;
export import di.bench.compile99_seq.trait.trait36;

namespace di::bench::compile99_seq {

export
struct Node36 : di::Node
{
    using Depends = di::Depends<trait::Trait35>;
    using Traits = di::Traits<Node36, trait::Trait36>;

    int impl(this auto const& self, trait::Trait36::get)
    {
        return self.i + self.getNode(trait::trait35).get();
    }

    Node36() = default;
    int i = 36;
};

}
