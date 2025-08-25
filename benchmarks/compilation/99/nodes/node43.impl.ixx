module di.bench.compile.node43:impl;
import di.bench.compile.node43;

namespace di::bench::compile {

template<class Context>
int Node43::Node<Context>::impl(trait::Trait43::get) const
{
    return i + getNode(trait::trait42).get();
}

}