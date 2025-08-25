module di.bench.compile.node4:impl;
import di.bench.compile.node4;

namespace di::bench::compile {

template<class Context>
int Node4::Node<Context>::impl(trait::Trait4::get) const
{
    return i + getNode(trait::trait3).get();
}

}
