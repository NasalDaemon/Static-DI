export module di.bench.compile99.node21;

import di;
export import di.bench.compile99.trait.trait20;
export import di.bench.compile99.trait.trait21;

namespace di::bench::compile99 {

export
struct Node21
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait20>;
        using Traits  = di::Traits<Node, trait::Trait21>;

        int impl(trait::Trait21::get) const;

        Node() = default;
        int i = 21;
    };
};

}
