module di.bench.compile.node41:impl;
import di.bench.compile.node41;

namespace di::bench::compile {

template<class Context>
int Node41::Node<Context>::impl(trait::Trait41::get) const
{
    return i + getNode(trait::trait40).get();
}

}