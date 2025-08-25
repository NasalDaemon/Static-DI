module di.bench.compile.node49:impl;
import di.bench.compile.node49;

namespace di::bench::compile {

template<class Context>
int Node49::Node<Context>::impl(trait::Trait49::get) const
{
    return i + getNode(trait::trait48).get();
}

}