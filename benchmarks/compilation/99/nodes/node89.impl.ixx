module di.bench.compile.node89:impl;
import di.bench.compile.node89;

namespace di::bench::compile {

template<class Context>
int Node89::Node<Context>::impl(trait::Trait89::get) const
{
    return i + getNode(trait::trait88).get();
}

} // namespace di::bench::compile
