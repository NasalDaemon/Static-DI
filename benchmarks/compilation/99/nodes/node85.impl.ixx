module di.bench.compile.node85:impl;
import di.bench.compile.node85;

namespace di::bench::compile {

template<class Context>
int Node85::Node<Context>::impl(trait::Trait85::get) const
{
    return i + getNode(trait::trait84).get();
}

}
