export module di.bench.compile99.node65;

import di;
export import di.bench.compile99.trait.trait64;
export import di.bench.compile99.trait.trait65;

namespace di::bench::compile99 {

export
struct Node65
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait64>;
        using Traits  = di::Traits<Node, trait::Trait65>;

        int impl(trait::Trait65::get) const;

        Node() = default;
        int i = 65;
    };
};

}
