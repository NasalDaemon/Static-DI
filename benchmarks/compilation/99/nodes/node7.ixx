export module di.bench.compile.node7;

import di;
export import di.bench.compile.trait.trait6;
export import di.bench.compile.trait.trait7;

namespace di::bench::compile {

export
struct Node7
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait6>;
        using Traits  = di::Traits<Node, trait::Trait7>;

        int impl(trait::Trait7::get) const;

        Node() = default;
        int i = 7;
    };
};

}
