export module di.bench.compile99.node90;

import di;
export import di.bench.compile99.trait.trait89;
export import di.bench.compile99.trait.trait90;

namespace di::bench::compile99 {

export
struct Node90
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait89>;
        using Traits  = di::Traits<Node, trait::Trait90>;

        int impl(trait::Trait90::get) const;

        Node() = default;
        int i = 90;
    };
};

}
