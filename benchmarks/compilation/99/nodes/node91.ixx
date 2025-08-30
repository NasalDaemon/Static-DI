export module di.bench.compile99.node91;

import di;
export import di.bench.compile99.trait.trait90;
export import di.bench.compile99.trait.trait91;

namespace di::bench::compile99 {

export
struct Node91
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait90>;
        using Traits  = di::Traits<Node, trait::Trait91>;

        int impl(trait::Trait91::get) const;

        Node() = default;
        int i = 91;
    };
};

}
