module di.bench.compile99.node60:impl;
import di.bench.compile99.node60;

namespace di::bench::compile99 {

template<class Context>
int Node60::Node<Context>::impl(trait::Trait60::get) const
{
    return i + getNode(trait::trait59).get();
}

}
