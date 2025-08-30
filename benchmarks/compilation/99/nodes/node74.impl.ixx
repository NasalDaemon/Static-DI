module di.bench.compile99.node74:impl;
import di.bench.compile99.node74;

namespace di::bench::compile99 {

template<class Context>
int Node74::Node<Context>::impl(trait::Trait74::get) const
{
    return i + getNode(trait::trait73).get();
}

} // namespace di::bench::compile99
