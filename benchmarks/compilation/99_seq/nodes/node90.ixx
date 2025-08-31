export module di.bench.compile99_seq.node90;

import di;
export import di.bench.compile99_seq.trait.trait89;
export import di.bench.compile99_seq.trait.trait90;

namespace di::bench::compile99_seq {

export
struct Node90 : di::Node
{
    using Depends = di::Depends<trait::Trait89>;
    using Traits = di::Traits<Node90, trait::Trait90>;

    int impl(this auto const& self, trait::Trait90::get)
    {
        return self.i + self.getNode(trait::trait89).get();
    }

    Node90() = default;
    int i = 90;
};

}
