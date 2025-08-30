export module di.bench.compile99.node76;

import di;
export import di.bench.compile99.trait.trait75;
export import di.bench.compile99.trait.trait76;

namespace di::bench::compile99 {

export
struct Node76
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait75>;
        using Traits  = di::Traits<Node, trait::Trait76>;

        int impl(trait::Trait76::get) const;

        Node() = default;
        int i = 76;
    };
};

}
