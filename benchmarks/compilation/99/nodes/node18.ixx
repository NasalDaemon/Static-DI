export module di.bench.compile99.node18;

import di;
export import di.bench.compile99.trait.trait17;
export import di.bench.compile99.trait.trait18;

namespace di::bench::compile99 {

export
struct Node18
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait17>;
        using Traits  = di::Traits<Node, trait::Trait18>;

        int impl(trait::Trait18::get) const;

        Node() = default;
        int i = 18;
    };
};

}
