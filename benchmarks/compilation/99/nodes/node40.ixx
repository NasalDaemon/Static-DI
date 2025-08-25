export module di.bench.compile.node40;

import di;
export import di.bench.compile.trait.trait39;
export import di.bench.compile.trait.trait40;

namespace di::bench::compile {

export
struct Node40
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait39>;
        using Traits  = di::Traits<Node, trait::Trait40>;

        int impl(trait::Trait40::get) const;

        Node() = default;
        int i = 40;
    };
};

}
