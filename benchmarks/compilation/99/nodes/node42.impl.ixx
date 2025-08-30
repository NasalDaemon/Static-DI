module di.bench.compile99.node42:impl;
import di.bench.compile99.node42;

namespace di::bench::compile99 {

template<class Context>
int Node42::Node<Context>::impl(trait::Trait42::get) const
{
    return i + getNode(trait::trait41).get();
}

}
