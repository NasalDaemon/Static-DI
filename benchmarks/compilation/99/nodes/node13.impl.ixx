module di.bench.compile.node13:impl;
import di.bench.compile.node13;

namespace di::bench::compile {
template<class Context>
int Node13::Node<Context>::impl(trait::Trait13::get) const
{
    return i + getNode(trait::trait12).get();
}
}
