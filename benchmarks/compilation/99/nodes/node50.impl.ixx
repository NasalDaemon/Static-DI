module di.bench.compile99.node50:impl;
import di.bench.compile99.node50;

namespace di::bench::compile99 {

template<class Context>
int Node50::Node<Context>::impl(trait::Trait50::get) const
{
    return i + getNode(trait::trait49).get();
}

} // namespace di::bench::compile99
