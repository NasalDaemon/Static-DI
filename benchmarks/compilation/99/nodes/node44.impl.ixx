module di.bench.compile.node44:impl;
import di.bench.compile.node44;

namespace di::bench::compile {

template<class Context>
int Node44::Node<Context>::impl(trait::Trait44::get) const
{
    return i + getNode(trait::trait43).get();
}

}
