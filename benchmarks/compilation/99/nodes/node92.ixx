export module di.bench.compile99.node92;

import di;
export import di.bench.compile99.trait.trait91;
export import di.bench.compile99.trait.trait92;

namespace di::bench::compile99 {

export
struct Node92
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait91>;
        using Traits  = di::Traits<Node, trait::Trait92>;

        int impl(trait::Trait92::get) const;

        Node() = default;
        int i = 92;
    };
};

}
