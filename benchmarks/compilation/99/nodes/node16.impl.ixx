module di.bench.compile99.node16:impl;
import di.bench.compile99.node16;

namespace di::bench::compile99 {
template<class Context>
int Node16::Node<Context>::impl(trait::Trait16::get) const
{
    return i + getNode(trait::trait15).get();
}
}
