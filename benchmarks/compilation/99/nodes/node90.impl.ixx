module di.bench.compile99.node90:impl;
import di.bench.compile99.node90;

namespace di::bench::compile99 {

template<class Context>
int Node90::Node<Context>::impl(trait::Trait90::get) const
{
    return i + getNode(trait::trait89).get();
}

} // namespace di::bench::compile99
