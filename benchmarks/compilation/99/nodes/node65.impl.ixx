module di.bench.compile99.node65:impl;
import di.bench.compile99.node65;

namespace di::bench::compile {

template<class Context>
int Node65::Node<Context>::impl(trait::Trait65::get) const
{
    return i + getNode(trait::trait64).get();
}

} // namespace di::bench::compile
