export module di.bench.compile99.node35;

import di;
export import di.bench.compile99.trait.trait34;
export import di.bench.compile99.trait.trait35;

namespace di::bench::compile99 {

export
struct Node35
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait34>;
        using Traits  = di::Traits<Node, trait::Trait35>;

        int impl(trait::Trait35::get) const;

        Node() = default;
        int i = 35;
    };
};

}
