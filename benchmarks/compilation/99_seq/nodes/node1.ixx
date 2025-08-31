export module di.bench.compile99_seq.node1;

export import di.bench.compile99_seq.trait.trait1;
import di;

namespace di::bench::compile99_seq {

export
struct Node1 : di::Node
{
    using Traits = di::Traits<Node1, trait::Trait1>;

    int impl(trait::Trait1::get) const
    {
        return i;
    }

    Node1() = default;
    int i = 1;
};

}
