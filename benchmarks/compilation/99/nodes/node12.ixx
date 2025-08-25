export module di.bench.compile.node12;

import di;
export import di.bench.compile.trait.trait11;
export import di.bench.compile.trait.trait12;

namespace di::bench::compile {

export
struct Node12
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait11>;
        using Traits  = di::Traits<Node, trait::Trait12>;

        int impl(trait::Trait12::get) const;

        Node() = default;
        int i = 12;
    };
};

}
