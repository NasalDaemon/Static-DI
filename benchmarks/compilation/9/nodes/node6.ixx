export module di.bench.compile9.node6;

import di;
export import di.bench.compile9.trait.trait5;
export import di.bench.compile9.trait.trait6;

namespace di::bench::compile9 {

export
struct Node6
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait5>;
        using Traits  = di::Traits<Node, trait::Trait6>;

        int impl(trait::Trait6::get) const;

        Node() = default;
        int i = 6;
    };
};

}
