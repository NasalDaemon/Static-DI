export module di.bench.compile99_seq.node35;

import di;
export import di.bench.compile99_seq.trait.trait34;
export import di.bench.compile99_seq.trait.trait35;

namespace di::bench::compile99_seq {

export
struct Node35 : di::Node
{
    using Depends = di::Depends<trait::Trait34>;
    using Traits = di::Traits<Node35, trait::Trait35>;

    int impl(this auto const& self, trait::Trait35::get)
    {
        return self.i + self.getNode(trait::trait34).get();
    }

    Node35() = default;
    int i = 35;
};

}
