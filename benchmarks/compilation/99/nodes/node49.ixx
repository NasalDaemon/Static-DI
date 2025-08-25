export module di.bench.compile.node49;

import di;
export import di.bench.compile.trait.trait48;
export import di.bench.compile.trait.trait49;

namespace di::bench::compile {

export
struct Node49
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait48>;
        using Traits  = di::Traits<Node, trait::Trait49>;

        int impl(trait::Trait49::get) const;

        Node() = default;
        int i = 49;
    };
};

}
