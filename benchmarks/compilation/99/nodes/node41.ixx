export module di.bench.compile.node41;

import di;
export import di.bench.compile.trait.trait40;
export import di.bench.compile.trait.trait41;

namespace di::bench::compile {

export
struct Node41
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait40>;
        using Traits  = di::Traits<Node, trait::Trait41>;

        int impl(trait::Trait41::get) const;

        Node() = default;
        int i = 41;
    };
};

}
