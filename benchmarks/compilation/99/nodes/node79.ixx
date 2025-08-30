export module di.bench.compile99.node79;

import di;
export import di.bench.compile99.trait.trait78;
export import di.bench.compile99.trait.trait79;

namespace di::bench::compile99 {

export
struct Node79
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait78>;
        using Traits  = di::Traits<Node, trait::Trait79>;

        int impl(trait::Trait79::get) const;

        Node() = default;
        int i = 79;
    };
};

}
