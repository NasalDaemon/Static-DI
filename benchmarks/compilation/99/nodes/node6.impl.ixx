module di.bench.compile.node6:impl;
import di.bench.compile.node6;

namespace di::bench::compile {

template<class Context>
int Node6::Node<Context>::impl(trait::Trait6::get) const
{
    return i + getNode(trait::trait5).get();
}

}
