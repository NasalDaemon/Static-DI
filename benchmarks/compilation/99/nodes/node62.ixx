export module di.bench.compile99.node62;

import di;
export import di.bench.compile99.trait.trait61;
export import di.bench.compile99.trait.trait62;

namespace di::bench::compile99 {

export
struct Node62
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait61>;
        using Traits  = di::Traits<Node, trait::Trait62>;

        int impl(trait::Trait62::get) const;

        Node() = default;
        int i = 62;
    };
};

}
