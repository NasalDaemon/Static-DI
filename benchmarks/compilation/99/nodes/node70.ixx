export module di.bench.compile99.node70;

import di;
export import di.bench.compile99.trait.trait69;
export import di.bench.compile99.trait.trait70;

namespace di::bench::compile99 {

export
struct Node70
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait69>;
        using Traits  = di::Traits<Node, trait::Trait70>;

        int impl(trait::Trait70::get) const;

        Node() = default;
        int i = 70;
    };
};

}
