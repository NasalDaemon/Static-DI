export module di.bench.compile.node90;

import di;
export import di.bench.compile.trait.trait89;
export import di.bench.compile.trait.trait90;

namespace di::bench::compile {

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
