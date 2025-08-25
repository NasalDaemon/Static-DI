module di.bench.compile.node68:impl;
import di.bench.compile.node68;

namespace di::bench::compile {

template<class Context>
int Node68::Node<Context>::impl(trait::Trait68::get) const
{
    return i + getNode(trait::trait67).get();
}

}
