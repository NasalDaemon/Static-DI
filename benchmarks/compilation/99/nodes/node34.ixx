export module di.bench.compile99.node34;

import di;
export import di.bench.compile99.trait.trait33;
export import di.bench.compile99.trait.trait34;

namespace di::bench::compile99 {

export
struct Node34
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait33>;
        using Traits  = di::Traits<Node, trait::Trait34>;

        int impl(trait::Trait34::get) const;

        Node() = default;
        int i = 34;
    };
};

}
