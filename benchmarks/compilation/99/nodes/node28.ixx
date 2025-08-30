export module di.bench.compile99.node28;

import di;
export import di.bench.compile99.trait.trait27;
export import di.bench.compile99.trait.trait28;

namespace di::bench::compile99 {

export
struct Node28
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait27>;
        using Traits  = di::Traits<Node, trait::Trait28>;

        int impl(trait::Trait28::get) const;

        Node() = default;
        int i = 28;
    };
};

}
