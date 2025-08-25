export module di.bench.compile.node71;

import di;
export import di.bench.compile.trait.trait70;
export import di.bench.compile.trait.trait71;

namespace di::bench::compile {

export
struct Node71
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait70>;
        using Traits  = di::Traits<Node, trait::Trait71>;

        int impl(trait::Trait71::get) const;

        Node() = default;
        int i = 71;
    };
};

}
