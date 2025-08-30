export module di.bench.compile99.node64;

import di;
export import di.bench.compile99.trait.trait63;
export import di.bench.compile99.trait.trait64;

namespace di::bench::compile99 {

export
struct Node64
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait63>;
        using Traits  = di::Traits<Node, trait::Trait64>;

        int impl(trait::Trait64::get) const;

        Node() = default;
        int i = 64;
    };
};

}
