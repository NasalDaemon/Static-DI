export module di.bench.compile99.node25;

import di;
export import di.bench.compile99.trait.trait24;
export import di.bench.compile99.trait.trait25;

namespace di::bench::compile99 {

export
struct Node25
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait24>;
        using Traits  = di::Traits<Node, trait::Trait25>;

        int impl(trait::Trait25::get) const;

        Node() = default;
        int i = 25;
    };
};

}
