module di.bench.compile99.node81:impl;
import di.bench.compile99.node81;

namespace di::bench::compile {

template<class Context>
int Node81::Node<Context>::impl(trait::Trait81::get) const
{
    return i + getNode(trait::trait80).get();
}

}
