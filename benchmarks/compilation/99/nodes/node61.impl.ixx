module di.bench.compile.node61:impl;
import di.bench.compile.node61;

namespace di::bench::compile {

template<class Context>
int Node61::Node<Context>::impl(trait::Trait61::get) const
{
    return i + getNode(trait::trait60).get();
}

}
