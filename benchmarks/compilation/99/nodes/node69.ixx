export module di.bench.compile.node69;

import di;
export import di.bench.compile.trait.trait68;
export import di.bench.compile.trait.trait69;

namespace di::bench::compile {

export
struct Node69
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait68>;
        using Traits  = di::Traits<Node, trait::Trait69>;

        int impl(trait::Trait69::get) const;

        Node() = default;
        int i = 69;
    };
};

}
