export module di.bench.compile.node24;

import di;
export import di.bench.compile.trait.trait23;
export import di.bench.compile.trait.trait24;

namespace di::bench::compile {

export
struct Node24
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait23>;
        using Traits  = di::Traits<Node, trait::Trait24>;

        int impl(trait::Trait24::get) const;

        Node() = default;
        int i = 24;
    };
};

}
