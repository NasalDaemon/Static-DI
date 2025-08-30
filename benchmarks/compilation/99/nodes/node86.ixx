export module di.bench.compile99.node86;

import di;
export import di.bench.compile99.trait.trait85;
export import di.bench.compile99.trait.trait86;

namespace di::bench::compile99 {

export
struct Node86
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait85>;
        using Traits  = di::Traits<Node, trait::Trait86>;

        int impl(trait::Trait86::get) const;

        Node() = default;
        int i = 86;
    };
};

}
