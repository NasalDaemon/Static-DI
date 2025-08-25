export module di.bench.compile.node74;

import di;
export import di.bench.compile.trait.trait73;
export import di.bench.compile.trait.trait74;

namespace di::bench::compile {

export
struct Node74
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait73>;
        using Traits  = di::Traits<Node, trait::Trait74>;

        int impl(trait::Trait74::get) const;

        Node() = default;
        int i = 74;
    };
};

}
