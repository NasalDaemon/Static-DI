export module di.bench.compile.node5;

import di;
export import di.bench.compile.trait.trait4;
export import di.bench.compile.trait.trait5;

namespace di::bench::compile {

export
struct Node5
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait4>;
        using Traits  = di::Traits<Node, trait::Trait5>;

        int impl(trait::Trait5::get) const;

        Node() = default;
        int i = 5;
    };
};

}
