export module di.bench.compile.node48;

import di;
export import di.bench.compile.trait.trait47;
export import di.bench.compile.trait.trait48;

namespace di::bench::compile {

export
struct Node48
{
    template<class Context>
    struct Node : di::Node
    {
        using Depends = di::Depends<trait::Trait47>;
        using Traits  = di::Traits<Node, trait::Trait48>;

        int impl(trait::Trait48::get) const;

        Node() = default;
        int i = 48;
    };
};

}
