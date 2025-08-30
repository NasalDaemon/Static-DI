export module di.bench.compile99.node19;

import di;
export import di.bench.compile99.trait.trait18;
export import di.bench.compile99.trait.trait19;

namespace di::bench::compile99 {

export
struct Node19
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait18>;
        using Traits  = di::Traits<Node, trait::Trait19>;

        int impl(trait::Trait19::get) const;

        Node() = default;
        int i = 19;
    };
};

}
