export module di.bench.compile99.node89;

import di;
export import di.bench.compile99.trait.trait88;
export import di.bench.compile99.trait.trait89;

namespace di::bench::compile99 {

export
struct Node89
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait88>;
        using Traits  = di::Traits<Node, trait::Trait89>;

        int impl(trait::Trait89::get) const;

        Node() = default;
        int i = 89;
    };
};

}
