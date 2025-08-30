module di.bench.compile99.node30:impl;
import di.bench.compile99.node30;

namespace di::bench::compile99 {

template<class Context>
int Node30::Node<Context>::impl(trait::Trait30::get) const
{
    return i + getNode(trait::trait29).get();
}

}
