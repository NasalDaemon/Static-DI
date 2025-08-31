export module di.bench.compile99_seq.node47;

import di;
export import di.bench.compile99_seq.trait.trait46;
export import di.bench.compile99_seq.trait.trait47;

namespace di::bench::compile99_seq {

export
struct Node47 : di::Node
{
    using Depends = di::Depends<trait::Trait46>;
    using Traits = di::Traits<Node47, trait::Trait47>;

    int impl(this auto const& self, trait::Trait47::get)
    {
        return self.i + self.getNode(trait::trait46).get();
    }

    Node47() = default;
    int i = 47;
};

}
