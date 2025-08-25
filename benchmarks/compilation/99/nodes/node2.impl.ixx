module di.bench.compile.node2:impl;

import di.bench.compile.node2;
import di;

namespace di::bench::compile {

template<class Context>
int Node2::Node<Context>::impl(trait::Trait2::get) const
{
    return i + getNode(trait::trait1).get();
}

}
