module di.bench.compile99.node36:impl;
import di.bench.compile99.node36;

namespace di::bench::compile99 {

template<class Context>
int Node36::Node<Context>::impl(trait::Trait36::get) const
{
    return i + getNode(trait::trait35).get();
}

} // namespace di::bench::compile99
