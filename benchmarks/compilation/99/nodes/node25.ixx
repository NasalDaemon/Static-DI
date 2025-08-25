export module di.bench.compile.node25;

import di;
export import di.bench.compile.trait.trait24;
export import di.bench.compile.trait.trait25;

namespace di::bench::compile {

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
