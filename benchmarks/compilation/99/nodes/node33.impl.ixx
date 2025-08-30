module di.bench.compile99.node33:impl;
import di.bench.compile99.node33;

namespace di::bench::compile99 {

template<class Context>
int Node33::Node<Context>::impl(trait::Trait33::get) const
{
    return i + getNode(trait::trait32).get();
}

}
