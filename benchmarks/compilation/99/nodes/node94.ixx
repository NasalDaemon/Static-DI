export module di.bench.compile99.node94;

import di;
export import di.bench.compile99.trait.trait93;
export import di.bench.compile99.trait.trait94;

namespace di::bench::compile99 {

export
struct Node94
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait93>;
        using Traits  = di::Traits<Node, trait::Trait94>;

        int impl(trait::Trait94::get) const;

        Node() = default;
        int i = 94;
    };
};

}
