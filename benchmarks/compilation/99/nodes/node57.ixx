export module di.bench.compile.node57;

import di;
export import di.bench.compile.trait.trait56;
export import di.bench.compile.trait.trait57;

namespace di::bench::compile {

export
struct Node57
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait56>;
        using Traits  = di::Traits<Node, trait::Trait57>;

        int impl(trait::Trait57::get) const;

        Node() = default;
        int i = 57;
    };
};

}
