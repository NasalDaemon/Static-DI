export module di.bench.compile.node51;

import di;
export import di.bench.compile.trait.trait50;
export import di.bench.compile.trait.trait51;

namespace di::bench::compile {

export
struct Node51
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait50>;
        using Traits  = di::Traits<Node, trait::Trait51>;

        int impl(trait::Trait51::get) const;

        Node() = default;
        int i = 51;
    };
};

}
