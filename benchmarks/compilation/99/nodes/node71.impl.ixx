module di.bench.compile99.node71:impl;
import di.bench.compile99.node71;

namespace di::bench::compile99 {

template<class Context>
int Node71::Node<Context>::impl(trait::Trait71::get) const
{
    return i + getNode(trait::trait70).get();
}

} // namespace di::bench::compile99
