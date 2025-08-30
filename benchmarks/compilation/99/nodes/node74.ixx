export module di.bench.compile99.node74;

import di;
export import di.bench.compile99.trait.trait73;
export import di.bench.compile99.trait.trait74;

namespace di::bench::compile99 {

export
struct Node74
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait73>;
        using Traits  = di::Traits<Node, trait::Trait74>;

        int impl(trait::Trait74::get) const;

        Node() = default;
        int i = 74;
    };
};

}
