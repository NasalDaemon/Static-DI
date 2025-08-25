export module di.bench.compile.node87;

import di;
export import di.bench.compile.trait.trait86;
export import di.bench.compile.trait.trait87;

namespace di::bench::compile {

export
struct Node87
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait86>;
        using Traits  = di::Traits<Node, trait::Trait87>;

        int impl(trait::Trait87::get) const;

        Node() = default;
        int i = 87;
    };
};

}
