module di.bench.compile.node42:impl;
import di.bench.compile.node42;

namespace di::bench::compile {

template<class Context>
int Node42::Node<Context>::impl(trait::Trait42::get) const
{
    return i + getNode(trait::trait41).get();
}

}
