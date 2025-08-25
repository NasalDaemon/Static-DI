module di.bench.compile.node94:impl;
import di.bench.compile.node94;

namespace di::bench::compile {

template<class Context>
int Node94::Node<Context>::impl(trait::Trait94::get) const
{
    return i + getNode(trait::trait93).get();
}

}