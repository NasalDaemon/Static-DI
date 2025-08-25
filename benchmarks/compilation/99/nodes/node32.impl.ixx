module di.bench.compile.node32:impl;
import di.bench.compile.node32;

namespace di::bench::compile {

template<class Context>
int Node32::Node<Context>::impl(trait::Trait32::get) const
{
    return i + getNode(trait::trait31).get();
}

}
