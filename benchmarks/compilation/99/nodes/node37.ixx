export module di.bench.compile.node37;

import di;
export import di.bench.compile.trait.trait36;
export import di.bench.compile.trait.trait37;

namespace di::bench::compile {

export
struct Node37
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait36>;
        using Traits  = di::Traits<Node, trait::Trait37>;

        int impl(trait::Trait37::get) const;

        Node() = default;
        int i = 37;
    };
};

}
