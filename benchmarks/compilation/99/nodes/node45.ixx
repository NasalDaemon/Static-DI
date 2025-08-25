export module di.bench.compile.node45;

import di;
export import di.bench.compile.trait.trait44;
export import di.bench.compile.trait.trait45;

namespace di::bench::compile {

export
struct Node45
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait44>;
        using Traits  = di::Traits<Node, trait::Trait45>;

        int impl(trait::Trait45::get) const;

        Node() = default;
        int i = 45;
    };
};

}
