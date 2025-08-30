module di.bench.compile99.node96:impl;
import di.bench.compile99.node96;

namespace di::bench::compile99 {

template<class Context>
int Node96::Node<Context>::impl(trait::Trait96::get) const
{
    return i + getNode(trait::trait95).get();
}

} // namespace di::bench::compile99
