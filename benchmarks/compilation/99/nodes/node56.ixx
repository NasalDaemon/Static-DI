export module di.bench.compile.node56;

import di;
export import di.bench.compile.trait.trait55;
export import di.bench.compile.trait.trait56;

namespace di::bench::compile {

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
