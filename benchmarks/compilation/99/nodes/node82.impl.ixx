module di.bench.compile99.node82:impl;
import di.bench.compile99.node82;

namespace di::bench::compile99 {

template<class Context>
int Node82::Node<Context>::impl(trait::Trait82::get) const
{
    return i + getNode(trait::trait81).get();
}

} // namespace di::bench::compile99
