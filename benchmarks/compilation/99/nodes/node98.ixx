export module di.bench.compile.node98;

import di;
export import di.bench.compile.trait.trait97;
export import di.bench.compile.trait.trait98;

namespace di::bench::compile {

export
struct Node98
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait97>;
        using Traits  = di::Traits<Node, trait::Trait98>;

        int impl(trait::Trait98::get) const;

        Node() = default;
        int i = 98;
    };
};

}
