export module di.bench.compile99.node31;

import di;
export import di.bench.compile99.trait.trait30;
export import di.bench.compile99.trait.trait31;

namespace di::bench::compile99 {

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
