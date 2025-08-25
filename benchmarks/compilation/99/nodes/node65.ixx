export module di.bench.compile.node65;

import di;
export import di.bench.compile.trait.trait64;
export import di.bench.compile.trait.trait65;

namespace di::bench::compile {

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
