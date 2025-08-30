export module di.bench.compile99.node3;

import di;
export import di.bench.compile99.trait.trait2;
export import di.bench.compile99.trait.trait3;

namespace di::bench::compile99 {

export
struct Node3
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait2>;
        using Traits  = di::Traits<Node, trait::Trait3>;

        int impl(trait::Trait3::get) const;

        Node() = default;
        int i = 3;
    };
};

}
