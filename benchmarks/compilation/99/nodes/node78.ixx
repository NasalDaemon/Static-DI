export module di.bench.compile.node78;

import di;
export import di.bench.compile.trait.trait77;
export import di.bench.compile.trait.trait78;

namespace di::bench::compile {

export
struct Node78
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait77>;
        using Traits  = di::Traits<Node, trait::Trait78>;

        int impl(trait::Trait78::get) const;

        Node() = default;
        int i = 78;
    };
};

}
