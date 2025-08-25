export module di.bench.compile.node63;

import di;
export import di.bench.compile.trait.trait62;
export import di.bench.compile.trait.trait63;

namespace di::bench::compile {

export
struct Node63
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait62>;
        using Traits  = di::Traits<Node, trait::Trait63>;

        int impl(trait::Trait63::get) const;

        Node() = default;
        int i = 63;
    };
};

}
