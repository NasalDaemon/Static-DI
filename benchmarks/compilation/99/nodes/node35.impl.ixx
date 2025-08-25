module di.bench.compile.node35:impl;
import di.bench.compile.node35;

namespace di::bench::compile {

template<class Context>
int Node35::Node<Context>::impl(trait::Trait35::get) const
{
    return i + getNode(trait::trait34).get();
}

}
