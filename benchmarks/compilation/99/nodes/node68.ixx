export module di.bench.compile.node68;

import di;
export import di.bench.compile.trait.trait67;
export import di.bench.compile.trait.trait68;

namespace di::bench::compile {

export
struct Node68
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait67>;
        using Traits  = di::Traits<Node, trait::Trait68>;

        int impl(trait::Trait68::get) const;

        Node() = default;
        int i = 68;
    };
};

}
