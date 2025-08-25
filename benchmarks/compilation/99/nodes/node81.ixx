export module di.bench.compile.node81;

import di;
export import di.bench.compile.trait.trait80;
export import di.bench.compile.trait.trait81;

namespace di::bench::compile {

export
struct Node81
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait80>;
        using Traits  = di::Traits<Node, trait::Trait81>;

        int impl(trait::Trait81::get) const;

        Node() = default;
        int i = 81;
    };
};

}
