export module di.bench.compile.node82;

import di;
export import di.bench.compile.trait.trait81;
export import di.bench.compile.trait.trait82;

namespace di::bench::compile {

export
struct Node82
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait81>;
        using Traits  = di::Traits<Node, trait::Trait82>;

        int impl(trait::Trait82::get) const;

        Node() = default;
        int i = 82;
    };
};

}
