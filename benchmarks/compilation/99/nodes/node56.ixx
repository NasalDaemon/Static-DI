export module di.bench.compile99.node56;

import di;
export import di.bench.compile99.trait.trait55;
export import di.bench.compile99.trait.trait56;

namespace di::bench::compile99 {

export
struct Node56
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait55>;
        using Traits  = di::Traits<Node, trait::Trait56>;

        int impl(trait::Trait56::get) const;

        Node() = default;
        int i = 56;
    };
};

}
