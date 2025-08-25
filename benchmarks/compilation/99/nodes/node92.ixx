export module di.bench.compile.node92;

import di;
export import di.bench.compile.trait.trait91;
export import di.bench.compile.trait.trait92;

namespace di::bench::compile {

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
