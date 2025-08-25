module di.bench.compile.node33:impl;
import di.bench.compile.node33;

namespace di::bench::compile {

template<class Context>
int Node33::Node<Context>::impl(trait::Trait33::get) const
{
    return i + getNode(trait::trait32).get();
}

}