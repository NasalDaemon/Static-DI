export module di.bench.compile.node58;

import di;
export import di.bench.compile.trait.trait57;
export import di.bench.compile.trait.trait58;

namespace di::bench::compile {

export
struct Node58
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait57>;
        using Traits  = di::Traits<Node, trait::Trait58>;

        int impl(trait::Trait58::get) const;

        Node() = default;
        int i = 58;
    };
};

}
