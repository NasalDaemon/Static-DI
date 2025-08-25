module di.bench.compile.node80:impl;
import di.bench.compile.node80;

namespace di::bench::compile {

template<class Context>
int Node80::Node<Context>::impl(trait::Trait80::get) const
{
    return i + getNode(trait::trait79).get();
}

}
