module di.bench.compile.node56:impl;
import di.bench.compile.node56;

namespace di::bench::compile {

template<class Context>
int Node56::Node<Context>::impl(trait::Trait56::get) const
{
    return i + getNode(trait::trait55).get();
}

}