export module di.bench.compile99.node23;

import di;
export import di.bench.compile99.trait.trait22;
export import di.bench.compile99.trait.trait23;

namespace di::bench::compile99 {

export
struct Node23
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait22>;
        using Traits  = di::Traits<Node, trait::Trait23>;

        int impl(trait::Trait23::get) const;

        Node() = default;
        int i = 23;
    };
};

}
