export module di.bench.compile99.node33;

import di;
export import di.bench.compile99.trait.trait32;
export import di.bench.compile99.trait.trait33;

namespace di::bench::compile99 {

export
struct Node33
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait32>;
        using Traits  = di::Traits<Node, trait::Trait33>;

        int impl(trait::Trait33::get) const;

        Node() = default;
        int i = 33;
    };
};

}
