module di.bench.compile99.node22:impl;
import di.bench.compile99.node22;

namespace di::bench::compile99 {

template<class Context>
int Node22::Node<Context>::impl(trait::Trait22::get) const
{
    return i + getNode(trait::trait21).get();
}

}
