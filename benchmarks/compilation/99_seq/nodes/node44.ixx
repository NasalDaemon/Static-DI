export module di.bench.compile99_seq.node44;

import di;
export import di.bench.compile99_seq.trait.trait43;
export import di.bench.compile99_seq.trait.trait44;

namespace di::bench::compile99_seq {

export
struct Node44 : di::Node
{
    using Depends = di::Depends<trait::Trait43>;
    using Traits = di::Traits<Node44, trait::Trait44>;

    int impl(this auto const& self, trait::Trait44::get)
    {
        return self.i + self.getNode(trait::trait43).get();
    }

    Node44() = default;
    int i = 44;
};

}
