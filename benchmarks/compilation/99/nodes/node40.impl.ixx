module di.bench.compile.node40:impl;
import di.bench.compile.node40;

namespace di::bench::compile {

template<class Context>
int Node40::Node<Context>::impl(trait::Trait40::get) const
{
    return i + getNode(trait::trait39).get();
}

}
