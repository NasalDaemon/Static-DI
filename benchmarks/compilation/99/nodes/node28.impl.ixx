module di.bench.compile99.node28:impl;
import di.bench.compile99.node28;

namespace di::bench::compile99 {

template<class Context>
int Node28::Node<Context>::impl(trait::Trait28::get) const
{
    return i + getNode(trait::trait27).get();
}

} // namespace di::bench::compile99
