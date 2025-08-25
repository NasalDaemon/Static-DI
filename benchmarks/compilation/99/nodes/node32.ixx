export module di.bench.compile.node32;

import di;
export import di.bench.compile.trait.trait31;
export import di.bench.compile.trait.trait32;

namespace di::bench::compile {

export
struct Node32
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait31>;
        using Traits  = di::Traits<Node, trait::Trait32>;

        int impl(trait::Trait32::get) const;

        Node() = default;
        int i = 32;
    };
};

}
