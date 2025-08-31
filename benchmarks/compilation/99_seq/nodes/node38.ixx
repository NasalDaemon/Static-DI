export module di.bench.compile99_seq.node38;

import di;
export import di.bench.compile99_seq.trait.trait37;
export import di.bench.compile99_seq.trait.trait38;

namespace di::bench::compile99_seq {

export
struct Node38 : di::Node
{
    using Depends = di::Depends<trait::Trait37>;
    using Traits = di::Traits<Node38, trait::Trait38>;

    int impl(this auto const& self, trait::Trait38::get)
    {
        return self.i + self.getNode(trait::trait37).get();
    }

    Node38() = default;
    int i = 38;
};

}
