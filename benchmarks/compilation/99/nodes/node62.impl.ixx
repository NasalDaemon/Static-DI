module di.bench.compile.node62:impl;
import di.bench.compile.node62;

namespace di::bench::compile {

template<class Context>
int Node62::Node<Context>::impl(trait::Trait62::get) const
{
    return i + getNode(trait::trait61).get();
}

}
