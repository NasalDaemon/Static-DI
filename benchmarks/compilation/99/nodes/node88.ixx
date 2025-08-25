export module di.bench.compile.node88;

import di;
export import di.bench.compile.trait.trait87;
export import di.bench.compile.trait.trait88;

namespace di::bench::compile {

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
