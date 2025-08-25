module di.bench.compile.node17:impl;
import di.bench.compile.node17;

namespace di::bench::compile {
template<class Context>
int Node17::Node<Context>::impl(trait::Trait17::get) const
{
    return i + getNode(trait::trait16).get();
}
}
