export module di.bench.compile99.node46;

import di;
export import di.bench.compile99.trait.trait45;
export import di.bench.compile99.trait.trait46;

namespace di::bench::compile99 {

export
struct Node46
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait45>;
        using Traits  = di::Traits<Node, trait::Trait46>;

        int impl(trait::Trait46::get) const;

        Node() = default;
        int i = 46;
    };
};

}
