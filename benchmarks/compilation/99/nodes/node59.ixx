export module di.bench.compile.node59;

import di;
export import di.bench.compile.trait.trait58;
export import di.bench.compile.trait.trait59;

namespace di::bench::compile {

export
struct Node59
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait58>;
        using Traits  = di::Traits<Node, trait::Trait59>;

        int impl(trait::Trait59::get) const;

        Node() = default;
        int i = 59;
    };
};

}
