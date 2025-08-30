module di.bench.compile99.node18:impl;
import di.bench.compile99.node18;

namespace di::bench::compile99 {
template<class Context>
int Node18::Node<Context>::impl(trait::Trait18::get) const
{
    return i + getNode(trait::trait17).get();
}
}
