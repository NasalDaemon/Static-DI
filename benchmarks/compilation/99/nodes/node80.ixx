export module di.bench.compile.node80;

import di;
export import di.bench.compile.trait.trait79;
export import di.bench.compile.trait.trait80;

namespace di::bench::compile {

export
struct Node80
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait79>;
        using Traits  = di::Traits<Node, trait::Trait80>;

        int impl(trait::Trait80::get) const;

        Node() = default;
        int i = 80;
    };
};

}
