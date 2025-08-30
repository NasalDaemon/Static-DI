export module di.bench.compile99.node38;

import di;
export import di.bench.compile99.trait.trait37;
export import di.bench.compile99.trait.trait38;

namespace di::bench::compile99 {

export
struct Node38
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait37>;
        using Traits  = di::Traits<Node, trait::Trait38>;

        int impl(trait::Trait38::get) const;

        Node() = default;
        int i = 38;
    };
};

}
