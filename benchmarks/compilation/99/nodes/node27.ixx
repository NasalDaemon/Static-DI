export module di.bench.compile99.node27;

import di;
export import di.bench.compile99.trait.trait26;
export import di.bench.compile99.trait.trait27;

namespace di::bench::compile99 {

export
struct Node27
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait26>;
        using Traits  = di::Traits<Node, trait::Trait27>;

        int impl(trait::Trait27::get) const;

        Node() = default;
        int i = 27;
    };
};

}
