export module di.bench.compile99_seq.node93;

import di;
export import di.bench.compile99_seq.trait.trait92;
export import di.bench.compile99_seq.trait.trait93;

namespace di::bench::compile99_seq {

export
struct Node93 : di::Node
{
    using Depends = di::Depends<trait::Trait92>;
    using Traits = di::Traits<Node93, trait::Trait93>;

    int impl(this auto const& self, trait::Trait93::get)
    {
        return self.i + self.getNode(trait::trait92).get();
    }

    Node93() = default;
    int i = 93;
};

}
