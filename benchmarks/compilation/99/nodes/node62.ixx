export module di.bench.compile.node62;

import di;
export import di.bench.compile.trait.trait61;
export import di.bench.compile.trait.trait62;

namespace di::bench::compile {

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
