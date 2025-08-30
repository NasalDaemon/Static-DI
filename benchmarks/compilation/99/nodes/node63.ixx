export module di.bench.compile99.node63;

import di;
export import di.bench.compile99.trait.trait62;
export import di.bench.compile99.trait.trait63;

namespace di::bench::compile99 {

export
struct Node63
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait62>;
        using Traits  = di::Traits<Node, trait::Trait63>;

        int impl(trait::Trait63::get) const;

        Node() = default;
        int i = 63;
    };
};

}
