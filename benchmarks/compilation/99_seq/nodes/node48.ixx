export module di.bench.compile99_seq.node48;

import di;
export import di.bench.compile99_seq.trait.trait47;
export import di.bench.compile99_seq.trait.trait48;

namespace di::bench::compile99_seq {

export
struct Node48 : di::Node
{
    using Depends = di::Depends<trait::Trait47>;
    using Traits = di::Traits<Node48, trait::Trait48>;

    int impl(this auto const& self, trait::Trait48::get)
    {
        return self.i + self.getNode(trait::trait47).get();
    }

    Node48() = default;
    int i = 48;
};

}
