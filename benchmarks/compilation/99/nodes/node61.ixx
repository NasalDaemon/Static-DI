export module di.bench.compile99.node61;

import di;
export import di.bench.compile99.trait.trait60;
export import di.bench.compile99.trait.trait61;

namespace di::bench::compile99 {

export
struct Node61
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait60>;
        using Traits  = di::Traits<Node, trait::Trait61>;

        int impl(trait::Trait61::get) const;

        Node() = default;
        int i = 61;
    };
};

}
