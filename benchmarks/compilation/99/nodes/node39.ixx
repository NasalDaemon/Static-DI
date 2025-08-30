export module di.bench.compile99.node39;

import di;
export import di.bench.compile99.trait.trait38;
export import di.bench.compile99.trait.trait39;

namespace di::bench::compile99 {

export
struct Node39
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait38>;
        using Traits  = di::Traits<Node, trait::Trait39>;

        int impl(trait::Trait39::get) const;

        Node() = default;
        int i = 39;
    };
};

}
