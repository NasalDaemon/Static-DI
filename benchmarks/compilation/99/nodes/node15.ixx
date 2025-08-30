export module di.bench.compile99.node15;

import di;
export import di.bench.compile99.trait.trait14;
export import di.bench.compile99.trait.trait15;

namespace di::bench::compile99 {

export
struct Node15
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait14>;
        using Traits  = di::Traits<Node, trait::Trait15>;

        int impl(trait::Trait15::get) const;

        Node() = default;
        int i = 15;
    };
};

}
