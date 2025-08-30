module di.bench.compile99.node91:impl;
import di.bench.compile99.node91;

namespace di::bench::compile99 {

template<class Context>
int Node91::Node<Context>::impl(trait::Trait91::get) const
{
    return i + getNode(trait::trait90).get();
}

}
