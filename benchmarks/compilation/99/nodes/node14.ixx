export module di.bench.compile99.node14;

import di;
export import di.bench.compile99.trait.trait13;
export import di.bench.compile99.trait.trait14;

namespace di::bench::compile99 {

export
struct Node14
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait13>;
        using Traits  = di::Traits<Node, trait::Trait14>;

        int impl(trait::Trait14::get) const;

        Node() = default;
        int i = 14;
    };
};

}
