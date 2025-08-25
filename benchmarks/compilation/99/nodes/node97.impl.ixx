module di.bench.compile.node97:impl;
import di.bench.compile.node97;

namespace di::bench::compile {

template<class Context>
int Node97::Node<Context>::impl(trait::Trait97::get) const
{
    return i + getNode(trait::trait96).get();
}

}
