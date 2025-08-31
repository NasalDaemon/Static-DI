export module di.bench.compile99_seq.node60;

import di;
export import di.bench.compile99_seq.trait.trait59;
export import di.bench.compile99_seq.trait.trait60;

namespace di::bench::compile99_seq {

export
struct Node60 : di::Node
{
    using Depends = di::Depends<trait::Trait59>;
    using Traits = di::Traits<Node60, trait::Trait60>;

    int impl(this auto const& self, trait::Trait60::get)
    {
        return self.i + self.getNode(trait::trait59).get();
    }

    Node60() = default;
    int i = 60;
};

}
