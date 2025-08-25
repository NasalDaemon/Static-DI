module di.bench.compile.node31:impl;
import di.bench.compile.node31;

namespace di::bench::compile {

template<class Context>
int Node31::Node<Context>::impl(trait::Trait31::get) const
{
    return i + getNode(trait::trait30).get();
}

}
