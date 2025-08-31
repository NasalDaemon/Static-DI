export module di.bench.compile99_seq.node49;

import di;
export import di.bench.compile99_seq.trait.trait48;
export import di.bench.compile99_seq.trait.trait49;

namespace di::bench::compile99_seq {

export
struct Node49 : di::Node
{
    using Depends = di::Depends<trait::Trait48>;
    using Traits = di::Traits<Node49, trait::Trait49>;

    int impl(this auto const& self, trait::Trait49::get)
    {
        return self.i + self.getNode(trait::trait48).get();
    }

    Node49() = default;
    int i = 49;
};

}
