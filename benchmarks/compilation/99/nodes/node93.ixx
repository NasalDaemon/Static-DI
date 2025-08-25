export module di.bench.compile.node93;

import di;
export import di.bench.compile.trait.trait92;
export import di.bench.compile.trait.trait93;

namespace di::bench::compile {

export
struct Node93
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait92>;
        using Traits  = di::Traits<Node, trait::Trait93>;

        int impl(trait::Trait93::get) const;

        Node() = default;
        int i = 93;
    };
};

}
