export module di.bench.compile99.node8;

import di;
export import di.bench.compile99.trait.trait7;
export import di.bench.compile99.trait.trait8;

namespace di::bench::compile99 {

export
struct Node8
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait7>;
        using Traits  = di::Traits<Node, trait::Trait8>;

        int impl(trait::Trait8::get) const;

        Node() = default;
        int i = 8;
    };
};

}
