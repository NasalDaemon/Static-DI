export module di.bench.compile99.node26;

import di;
export import di.bench.compile99.trait.trait25;
export import di.bench.compile99.trait.trait26;

namespace di::bench::compile99 {

export
struct Node26
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait25>;
        using Traits  = di::Traits<Node, trait::Trait26>;

        int impl(trait::Trait26::get) const;

        Node() = default;
        int i = 26;
    };
};

}
