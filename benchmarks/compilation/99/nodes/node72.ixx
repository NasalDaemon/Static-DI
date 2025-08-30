export module di.bench.compile99.node72;

import di;
export import di.bench.compile99.trait.trait71;
export import di.bench.compile99.trait.trait72;

namespace di::bench::compile99 {

export
struct Node72
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait71>;
        using Traits  = di::Traits<Node, trait::Trait72>;

        int impl(trait::Trait72::get) const;

        Node() = default;
        int i = 72;
    };
};

}
