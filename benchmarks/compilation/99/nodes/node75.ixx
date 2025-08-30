export module di.bench.compile99.node75;

import di;
export import di.bench.compile99.trait.trait74;
export import di.bench.compile99.trait.trait75;

namespace di::bench::compile99 {

export
struct Node75
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait74>;
        using Traits  = di::Traits<Node, trait::Trait75>;

        int impl(trait::Trait75::get) const;

        Node() = default;
        int i = 75;
    };
};

}
