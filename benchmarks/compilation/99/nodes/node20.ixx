export module di.bench.compile.node20;

import di;
export import di.bench.compile.trait.trait19;
export import di.bench.compile.trait.trait20;

namespace di::bench::compile {

export
struct Node20
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait19>;
        using Traits  = di::Traits<Node, trait::Trait20>;

        int impl(trait::Trait20::get) const;

        Node() = default;
        int i = 20;
    };
};

}
