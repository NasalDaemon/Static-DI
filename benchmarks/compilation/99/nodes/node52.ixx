export module di.bench.compile99.node52;

import di;
export import di.bench.compile99.trait.trait51;
export import di.bench.compile99.trait.trait52;

namespace di::bench::compile99 {

export
struct Node52
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait51>;
        using Traits  = di::Traits<Node, trait::Trait52>;

        int impl(trait::Trait52::get) const;

        Node() = default;
        int i = 52;
    };
};

}
