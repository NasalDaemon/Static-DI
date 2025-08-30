export module di.bench.compile99.node22;

import di;
export import di.bench.compile99.trait.trait21;
export import di.bench.compile99.trait.trait22;

namespace di::bench::compile99 {

export
struct Node22
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait21>;
        using Traits  = di::Traits<Node, trait::Trait22>;

        int impl(trait::Trait22::get) const;

        Node() = default;
        int i = 22;
    };
};

}
