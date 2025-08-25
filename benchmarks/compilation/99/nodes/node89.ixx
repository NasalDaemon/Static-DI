export module di.bench.compile.node89;

import di;
export import di.bench.compile.trait.trait88;
export import di.bench.compile.trait.trait89;

namespace di::bench::compile {

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
