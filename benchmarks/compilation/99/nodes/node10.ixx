export module di.bench.compile99.node10;

import di;
export import di.bench.compile99.trait.trait9;
export import di.bench.compile99.trait.trait10;

namespace di::bench::compile99 {

export
struct Node10
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait9>;
        using Traits  = di::Traits<Node, trait::Trait10>;

        int impl(trait::Trait10::get) const;

        Node() = default;
        int i = 10;
    };
};

}
