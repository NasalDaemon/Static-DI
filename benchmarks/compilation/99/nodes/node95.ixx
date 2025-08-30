export module di.bench.compile99.node95;

import di;
export import di.bench.compile99.trait.trait94;
export import di.bench.compile99.trait.trait95;

namespace di::bench::compile99 {

export
struct Node95
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait94>;
        using Traits  = di::Traits<Node, trait::Trait95>;

        int impl(trait::Trait95::get) const;

        Node() = default;
        int i = 95;
    };
};

}
