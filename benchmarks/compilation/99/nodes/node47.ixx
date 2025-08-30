export module di.bench.compile99.node47;

import di;
export import di.bench.compile99.trait.trait46;
export import di.bench.compile99.trait.trait47;

namespace di::bench::compile99 {

export
struct Node47
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait46>;
        using Traits  = di::Traits<Node, trait::Trait47>;

        int impl(trait::Trait47::get) const;

        Node() = default;
        int i = 47;
    };
};

}
