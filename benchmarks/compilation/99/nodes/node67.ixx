export module di.bench.compile99.node67;

import di;
export import di.bench.compile99.trait.trait66;
export import di.bench.compile99.trait.trait67;

namespace di::bench::compile99 {

export
struct Node67
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait66>;
        using Traits  = di::Traits<Node, trait::Trait67>;

        int impl(trait::Trait67::get) const;

        Node() = default;
        int i = 67;
    };
};

}
