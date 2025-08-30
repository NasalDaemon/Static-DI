export module di.bench.compile9.node5;

import di;
export import di.bench.compile9.trait.trait4;
export import di.bench.compile9.trait.trait5;

namespace di::bench::compile9 {

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
