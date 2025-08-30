export module di.bench.compile99.node84;

import di;
export import di.bench.compile99.trait.trait83;
export import di.bench.compile99.trait.trait84;

namespace di::bench::compile99 {

export
struct Node84
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait83>;
        using Traits  = di::Traits<Node, trait::Trait84>;

        int impl(trait::Trait84::get) const;

        Node() = default;
        int i = 84;
    };
};

}
