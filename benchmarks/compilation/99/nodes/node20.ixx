export module di.bench.compile99.node20;

import di;
export import di.bench.compile99.trait.trait19;
export import di.bench.compile99.trait.trait20;

namespace di::bench::compile99 {

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
