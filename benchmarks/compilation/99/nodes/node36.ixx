export module di.bench.compile99.node36;

import di;
export import di.bench.compile99.trait.trait35;
export import di.bench.compile99.trait.trait36;

namespace di::bench::compile99 {

export
struct Node36
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait35>;
        using Traits  = di::Traits<Node, trait::Trait36>;

        int impl(trait::Trait36::get) const;

        Node() = default;
        int i = 36;
    };
};

}
