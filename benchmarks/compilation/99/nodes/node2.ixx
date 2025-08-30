export module di.bench.compile99.node2;

import di;
export import di.bench.compile99.trait.trait1;
export import di.bench.compile99.trait.trait2;

namespace di::bench::compile99 {

export
struct Node2
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait1>;
        using Traits = di::Traits<Node, trait::Trait2>;

        int impl(trait::Trait2::get) const;

        Node() = default;
        int i = 2;
    };
};

}
