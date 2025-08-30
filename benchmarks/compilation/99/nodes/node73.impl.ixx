module di.bench.compile99.node73:impl;
import di.bench.compile99.node73;

namespace di::bench::compile99 {

template<class Context>
int Node73::Node<Context>::impl(trait::Trait73::get) const
{
    return i + getNode(trait::trait72).get();
}

} // namespace di::bench::compile99
