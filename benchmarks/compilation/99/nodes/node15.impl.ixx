module di.bench.compile99.node15:impl;
import di.bench.compile99.node15;

namespace di::bench::compile99 {
template<class Context>
int Node15::Node<Context>::impl(trait::Trait15::get) const
{
    return i + getNode(trait::trait14).get();
}
}
