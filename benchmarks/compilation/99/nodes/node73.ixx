export module di.bench.compile.node73;

import di;
export import di.bench.compile.trait.trait72;
export import di.bench.compile.trait.trait73;

namespace di::bench::compile {

export
struct Node73
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait72>;
        using Traits  = di::Traits<Node, trait::Trait73>;

        int impl(trait::Trait73::get) const;

        Node() = default;
        int i = 73;
    };
};

}
