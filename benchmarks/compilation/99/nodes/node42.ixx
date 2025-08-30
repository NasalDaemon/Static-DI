export module di.bench.compile99.node42;

import di;
export import di.bench.compile99.trait.trait41;
export import di.bench.compile99.trait.trait42;

namespace di::bench::compile99 {

export
struct Node42
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait41>;
        using Traits  = di::Traits<Node, trait::Trait42>;

        int impl(trait::Trait42::get) const;

        Node() = default;
        int i = 42;
    };
};

}
