export module di.bench.compile.node64;

import di;
export import di.bench.compile.trait.trait63;
export import di.bench.compile.trait.trait64;

namespace di::bench::compile {

export
struct Node64
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait63>;
        using Traits  = di::Traits<Node, trait::Trait64>;

        int impl(trait::Trait64::get) const;

        Node() = default;
        int i = 64;
    };
};

}
