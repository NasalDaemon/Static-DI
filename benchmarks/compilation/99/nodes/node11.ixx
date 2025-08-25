export module di.bench.compile.node11;

import di;
export import di.bench.compile.trait.trait10;
export import di.bench.compile.trait.trait11;

namespace di::bench::compile {

export
struct Node11
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait10>;
        using Traits  = di::Traits<Node, trait::Trait11>;

        int impl(trait::Trait11::get) const;

        Node() = default;
        int i = 11;
    };
};

}
