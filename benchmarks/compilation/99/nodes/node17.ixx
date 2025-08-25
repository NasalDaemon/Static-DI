export module di.bench.compile.node17;

import di;
export import di.bench.compile.trait.trait16;
export import di.bench.compile.trait.trait17;

namespace di::bench::compile {

export
struct Node17
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait16>;
        using Traits  = di::Traits<Node, trait::Trait17>;

        int impl(trait::Trait17::get) const;

        Node() = default;
        int i = 17;
    };
};

}
