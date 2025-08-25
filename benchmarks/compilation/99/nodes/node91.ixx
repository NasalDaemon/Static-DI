export module di.bench.compile.node91;

import di;
export import di.bench.compile.trait.trait90;
export import di.bench.compile.trait.trait91;

namespace di::bench::compile {

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
