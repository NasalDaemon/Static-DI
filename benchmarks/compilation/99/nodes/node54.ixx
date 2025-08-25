export module di.bench.compile.node54;

import di;
export import di.bench.compile.trait.trait53;
export import di.bench.compile.trait.trait54;

namespace di::bench::compile {

export
struct Node54
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait53>;
        using Traits  = di::Traits<Node, trait::Trait54>;

        int impl(trait::Trait54::get) const;

        Node() = default;
        int i = 54;
    };
};

}
