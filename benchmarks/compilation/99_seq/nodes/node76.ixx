export module di.bench.compile99_seq.node76;

import di;
export import di.bench.compile99_seq.trait.trait75;
export import di.bench.compile99_seq.trait.trait76;

namespace di::bench::compile99_seq {

export
struct Node76 : di::Node
{
    using Depends = di::Depends<trait::Trait75>;
    using Traits = di::Traits<Node76, trait::Trait76>;

    int impl(this auto const& self, trait::Trait76::get)
    {
        return self.i + self.getNode(trait::trait75).get();
    }

    Node76() = default;
    int i = 76;
};

}
