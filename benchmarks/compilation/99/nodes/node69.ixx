export module di.bench.compile99.node69;

import di;
export import di.bench.compile99.trait.trait68;
export import di.bench.compile99.trait.trait69;

namespace di::bench::compile99 {

export
struct Node69
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait68>;
        using Traits  = di::Traits<Node, trait::Trait69>;

        int impl(trait::Trait69::get) const;

        Node() = default;
        int i = 69;
    };
};

}
