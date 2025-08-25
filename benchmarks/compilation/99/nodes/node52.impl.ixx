module di.bench.compile.node52:impl;
import di.bench.compile.node52;

namespace di::bench::compile {

template<class Context>
int Node52::Node<Context>::impl(trait::Trait52::get) const
{
    return i + getNode(trait::trait51).get();
}

}
