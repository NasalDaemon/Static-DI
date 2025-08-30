export module di.bench.compile99.node71;

import di;
export import di.bench.compile99.trait.trait70;
export import di.bench.compile99.trait.trait71;

namespace di::bench::compile99 {

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
