export module di.bench.compile.node83;

import di;
export import di.bench.compile.trait.trait82;
export import di.bench.compile.trait.trait83;

namespace di::bench::compile {

export
struct Node83
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait82>;
        using Traits  = di::Traits<Node, trait::Trait83>;

        int impl(trait::Trait83::get) const;

        Node() = default;
        int i = 83;
    };
};

}
