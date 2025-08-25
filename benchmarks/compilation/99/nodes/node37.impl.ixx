module di.bench.compile.node37:impl;
import di.bench.compile.node37;

namespace di::bench::compile {

template<class Context>
int Node37::Node<Context>::impl(trait::Trait37::get) const
{
    return i + getNode(trait::trait36).get();
}

}
