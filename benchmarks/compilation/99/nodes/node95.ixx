export module di.bench.compile.node95;

import di;
export import di.bench.compile.trait.trait94;
export import di.bench.compile.trait.trait95;

namespace di::bench::compile {

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
