module di.bench.compile.node57:impl;
import di.bench.compile.node57;

namespace di::bench::compile {

template<class Context>
int Node57::Node<Context>::impl(trait::Trait57::get) const
{
    return i + getNode(trait::trait56).get();
}

}