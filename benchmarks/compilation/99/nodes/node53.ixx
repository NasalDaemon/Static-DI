export module di.bench.compile.node53;

import di;
export import di.bench.compile.trait.trait52;
export import di.bench.compile.trait.trait53;

namespace di::bench::compile {

export
struct Node53
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait52>;
        using Traits  = di::Traits<Node, trait::Trait53>;

        int impl(trait::Trait53::get) const;

        Node() = default;
        int i = 53;
    };
};

}
