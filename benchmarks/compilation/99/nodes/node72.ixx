export module di.bench.compile.node72;

import di;
export import di.bench.compile.trait.trait71;
export import di.bench.compile.trait.trait72;

namespace di::bench::compile {

export
struct Node72
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait71>;
        using Traits  = di::Traits<Node, trait::Trait72>;

        int impl(trait::Trait72::get) const;

        Node() = default;
        int i = 72;
    };
};

}
