export module di.bench.compile99.node50;

import di;
export import di.bench.compile99.trait.trait49;
export import di.bench.compile99.trait.trait50;

namespace di::bench::compile99 {

export
struct Node50
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait49>;
        using Traits  = di::Traits<Node, trait::Trait50>;

        int impl(trait::Trait50::get) const;

        Node() = default;
        int i = 50;
    };
};

}
