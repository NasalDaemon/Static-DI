module di.bench.compile.node14:impl;
import di.bench.compile.node14;

namespace di::bench::compile {
template<class Context>
int Node14::Node<Context>::impl(trait::Trait14::get) const
{
    return i + getNode(trait::trait13).get();
}
}
