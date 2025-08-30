export module di.bench.compile99.node44;

import di;
export import di.bench.compile99.trait.trait43;
export import di.bench.compile99.trait.trait44;

namespace di::bench::compile99 {

export
struct Node44
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait43>;
        using Traits  = di::Traits<Node, trait::Trait44>;

        int impl(trait::Trait44::get) const;

        Node() = default;
        int i = 44;
    };
};

}
