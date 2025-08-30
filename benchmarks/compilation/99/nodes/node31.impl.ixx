module di.bench.compile99.node31:impl;
import di.bench.compile99.node31;

namespace di::bench::compile99 {

template<class Context>
int Node31::Node<Context>::impl(trait::Trait31::get) const
{
    return i + getNode(trait::trait30).get();
}

}
