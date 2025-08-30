export module di.bench.compile99.node96;

import di;
export import di.bench.compile99.trait.trait95;
export import di.bench.compile99.trait.trait96;

namespace di::bench::compile99 {

export
struct Node96
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait95>;
        using Traits  = di::Traits<Node, trait::Trait96>;

        int impl(trait::Trait96::get) const;

        Node() = default;
        int i = 96;
    };
};

}
