module di.bench.compile.node39:impl;
import di.bench.compile.node39;

namespace di::bench::compile {

template<class Context>
int Node39::Node<Context>::impl(trait::Trait39::get) const
{
    return i + getNode(trait::trait38).get();
}

}