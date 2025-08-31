export module di.bench.compile99_seq.node99;

import di;
export import di.bench.compile99_seq.trait.trait98;
export import di.bench.compile99_seq.trait.trait99;

namespace di::bench::compile99_seq {

export
struct Node99 : di::Node
{
    using Depends = di::Depends<trait::Trait98>;
    using Traits = di::Traits<Node99, trait::Trait99>;

    int impl(this auto const& self, trait::Trait99::get)
    {
        return self.i + self.getNode(trait::trait98).get();
    }

    Node99() = default;
    int i = 99;
};

}
