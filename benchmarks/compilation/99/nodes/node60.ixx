export module di.bench.compile.node60;

import di;
export import di.bench.compile.trait.trait59;
export import di.bench.compile.trait.trait60;

namespace di::bench::compile {

export
struct Node60
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait59>;
        using Traits  = di::Traits<Node, trait::Trait60>;

        int impl(trait::Trait60::get) const;

        Node() = default;
        int i = 60;
    };
};

}
