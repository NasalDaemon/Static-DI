module di.bench.compile.node45:impl;
import di.bench.compile.node45;

namespace di::bench::compile {

template<class Context>
int Node45::Node<Context>::impl(trait::Trait45::get) const
{
    return i + getNode(trait::trait44).get();
}

}
