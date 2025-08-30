export module di.bench.compile99.node55;

import di;
export import di.bench.compile99.trait.trait54;
export import di.bench.compile99.trait.trait55;

namespace di::bench::compile99 {

export
struct Node55
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait54>;
        using Traits  = di::Traits<Node, trait::Trait55>;

        int impl(trait::Trait55::get) const;

        Node() = default;
        int i = 55;
    };
};

}
