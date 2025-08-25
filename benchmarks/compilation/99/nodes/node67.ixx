export module di.bench.compile.node67;

import di;
export import di.bench.compile.trait.trait66;
export import di.bench.compile.trait.trait67;

namespace di::bench::compile {

export
struct Node67
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait66>;
        using Traits  = di::Traits<Node, trait::Trait67>;

        int impl(trait::Trait67::get) const;

        Node() = default;
        int i = 67;
    };
};

}
