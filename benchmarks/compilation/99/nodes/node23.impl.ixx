module di.bench.compile.node23:impl;
import di.bench.compile.node23;

namespace di::bench::compile {

template<class Context>
int Node23::Node<Context>::impl(trait::Trait23::get) const
{
    return i + getNode(trait::trait22).get();
}

}