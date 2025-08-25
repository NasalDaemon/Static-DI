module di.bench.compile.node48:impl;
import di.bench.compile.node48;

namespace di::bench::compile {

template<class Context>
int Node48::Node<Context>::impl(trait::Trait48::get) const
{
    return i + getNode(trait::trait47).get();
}

}