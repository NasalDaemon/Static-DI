module di.bench.compile99.node40:impl;
import di.bench.compile99.node40;

namespace di::bench::compile99 {

template<class Context>
int Node40::Node<Context>::impl(trait::Trait40::get) const
{
    return i + getNode(trait::trait39).get();
}

}
