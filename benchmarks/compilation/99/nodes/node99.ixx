export module di.bench.compile.node99;

import di;
export import di.bench.compile.trait.trait98;
export import di.bench.compile.trait.trait99;

namespace di::bench::compile {

export
struct Node99
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait98>;
        using Traits  = di::Traits<Node, trait::Trait99>;

        int impl(trait::Trait99::get) const;

        Node() = default;
        int i = 99;
    };
};

}
