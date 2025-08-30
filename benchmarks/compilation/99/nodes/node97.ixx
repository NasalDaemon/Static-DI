export module di.bench.compile99.node97;

import di;
export import di.bench.compile99.trait.trait96;
export import di.bench.compile99.trait.trait97;

namespace di::bench::compile99 {

export
struct Node97
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait96>;
        using Traits  = di::Traits<Node, trait::Trait97>;

        int impl(trait::Trait97::get) const;

        Node() = default;
        int i = 97;
    };
};

}
