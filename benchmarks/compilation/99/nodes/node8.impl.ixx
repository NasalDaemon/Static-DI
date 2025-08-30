module di.bench.compile99.node8:impl;
import di.bench.compile99.node8;

namespace di::bench::compile99 {

template<class Context>
int Node8::Node<Context>::impl(trait::Trait8::get) const
{
    return i + getNode(trait::trait7).get();
}

}
