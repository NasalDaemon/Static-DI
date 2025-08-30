export module di.bench.compile99.node85;

import di;
export import di.bench.compile99.trait.trait84;
export import di.bench.compile99.trait.trait85;

namespace di::bench::compile99 {

export
struct Node85
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait84>;
        using Traits  = di::Traits<Node, trait::Trait85>;

        int impl(trait::Trait85::get) const;

        Node() = default;
        int i = 85;
    };
};

}
