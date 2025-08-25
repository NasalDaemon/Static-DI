module di.bench.compile.node79:impl;
import di.bench.compile.node79;

namespace di::bench::compile {

template<class Context>
int Node79::Node<Context>::impl(trait::Trait79::get) const
{
    return i + getNode(trait::trait78).get();
}

}
