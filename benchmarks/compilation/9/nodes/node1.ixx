export module di.bench.compile9.node1;

export import di.bench.compile9.trait.trait1;
import di;

namespace di::bench::compile9 {

export
struct Node1
{
    template<class Context>
    struct Node : di::Node
    {
        using Traits = di::Traits<Node, trait::Trait1>;

        int impl(trait::Trait1::get) const;

        Node() = default;
        int i = 1;
    };
};

}
