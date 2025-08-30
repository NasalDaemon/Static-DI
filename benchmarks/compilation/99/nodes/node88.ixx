export module di.bench.compile99.node88;

import di;
export import di.bench.compile99.trait.trait87;
export import di.bench.compile99.trait.trait88;

namespace di::bench::compile99 {

export
struct Node88
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait87>;
        using Traits  = di::Traits<Node, trait::Trait88>;

        int impl(trait::Trait88::get) const;

        Node() = default;
        int i = 88;
    };
};

}
