export module di.bench.compile.node66;

import di;
export import di.bench.compile.trait.trait65;
export import di.bench.compile.trait.trait66;

namespace di::bench::compile {

export
struct Node66
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait65>;
        using Traits  = di::Traits<Node, trait::Trait66>;

        int impl(trait::Trait66::get) const;

        Node() = default;
        int i = 66;
    };
};

}
