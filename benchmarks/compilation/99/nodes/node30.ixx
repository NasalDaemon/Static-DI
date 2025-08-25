export module di.bench.compile.node30;

import di;
export import di.bench.compile.trait.trait29;
export import di.bench.compile.trait.trait30;

namespace di::bench::compile {

export
struct Node30
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait29>;
        using Traits  = di::Traits<Node, trait::Trait30>;

        int impl(trait::Trait30::get) const;

        Node() = default;
        int i = 30;
    };
};

}
