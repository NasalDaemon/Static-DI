export module di.bench.compile.node77;

import di;
export import di.bench.compile.trait.trait76;
export import di.bench.compile.trait.trait77;

namespace di::bench::compile {

export
struct Node77
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait76>;
        using Traits  = di::Traits<Node, trait::Trait77>;

        int impl(trait::Trait77::get) const;

        Node() = default;
        int i = 77;
    };
};

}
