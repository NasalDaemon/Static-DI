module di.bench.compile.node1:impl;

import di.bench.compile.node1;
import di;

namespace di::bench::compile {

template<class Context>
int Node1::Node<Context>::impl(trait::Trait1::get) const
{
    return i;
}

}
