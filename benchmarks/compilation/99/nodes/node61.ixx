export module di.bench.compile.node61;

import di;
export import di.bench.compile.trait.trait60;
export import di.bench.compile.trait.trait61;

namespace di::bench::compile {

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
