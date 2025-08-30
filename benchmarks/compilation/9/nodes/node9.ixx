export module di.bench.compile9.node9;

import di;
export import di.bench.compile9.trait.trait8;
export import di.bench.compile9.trait.trait9;

namespace di::bench::compile9 {

export
struct Node9
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait8>;
        using Traits  = di::Traits<Node, trait::Trait9>;

        int impl(trait::Trait9::get) const;

        Node() = default;
        int i = 9;
    };
};

}
