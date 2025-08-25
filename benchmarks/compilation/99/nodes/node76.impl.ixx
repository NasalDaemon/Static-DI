module di.bench.compile.node76:impl;
import di.bench.compile.node76;

namespace di::bench::compile {

template<class Context>
int Node76::Node<Context>::impl(trait::Trait76::get) const
{
    return i + getNode(trait::trait75).get();
}

}
