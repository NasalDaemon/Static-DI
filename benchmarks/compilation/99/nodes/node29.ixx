export module di.bench.compile.node29;

import di;
export import di.bench.compile.trait.trait28;
export import di.bench.compile.trait.trait29;

namespace di::bench::compile {

export
struct Node29
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait28>;
        using Traits  = di::Traits<Node, trait::Trait29>;

        int impl(trait::Trait29::get) const;

        Node() = default;
        int i = 29;
    };
};

}
