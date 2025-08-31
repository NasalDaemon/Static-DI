export module di.bench.compile99_seq.node55;

import di;
export import di.bench.compile99_seq.trait.trait54;
export import di.bench.compile99_seq.trait.trait55;

namespace di::bench::compile99_seq {

export
struct Node55 : di::Node
{
    using Depends = di::Depends<trait::Trait54>;
    using Traits = di::Traits<Node55, trait::Trait55>;

    int impl(this auto const& self, trait::Trait55::get)
    {
        return self.i + self.getNode(trait::trait54).get();
    }

    Node55() = default;
    int i = 55;
};

}
