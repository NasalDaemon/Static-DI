export module di.bench.compile.node31;

import di;
export import di.bench.compile.trait.trait30;
export import di.bench.compile.trait.trait31;

namespace di::bench::compile {

export
struct Node31
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait30>;
        using Traits  = di::Traits<Node, trait::Trait31>;

        int impl(trait::Trait31::get) const;

        Node() = default;
        int i = 31;
    };
};

}
