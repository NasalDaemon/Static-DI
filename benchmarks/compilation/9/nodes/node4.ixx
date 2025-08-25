export module di.bench.compile.node4;

import di;
export import di.bench.compile.trait.trait3;
export import di.bench.compile.trait.trait4;

namespace di::bench::compile {

export
struct Node4
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait3>;
        using Traits  = di::Traits<Node, trait::Trait4>;

        int impl(trait::Trait4::get) const;

        Node() = default;
        int i = 4;
    };
};

}
