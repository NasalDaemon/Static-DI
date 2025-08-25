export module di.bench.compile.node43;

import di;
export import di.bench.compile.trait.trait42;
export import di.bench.compile.trait.trait43;

namespace di::bench::compile {

export
struct Node43
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait42>;
        using Traits  = di::Traits<Node, trait::Trait43>;

        int impl(trait::Trait43::get) const;

        Node() = default;
        int i = 43;
    };
};

}
