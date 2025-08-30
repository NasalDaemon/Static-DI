export module di.bench.compile99.node13;

import di;
export import di.bench.compile99.trait.trait12;
export import di.bench.compile99.trait.trait13;

namespace di::bench::compile99 {

export
struct Node13
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait12>;
        using Traits  = di::Traits<Node, trait::Trait13>;

        int impl(trait::Trait13::get) const;

        Node() = default;
        int i = 13;
    };
};

}
