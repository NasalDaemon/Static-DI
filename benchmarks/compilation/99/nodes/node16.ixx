export module di.bench.compile.node16;

import di;
export import di.bench.compile.trait.trait15;
export import di.bench.compile.trait.trait16;

namespace di::bench::compile {

export
struct Node16
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait15>;
        using Traits  = di::Traits<Node, trait::Trait16>;

        int impl(trait::Trait16::get) const;

        Node() = default;
        int i = 16;
    };
};

}
