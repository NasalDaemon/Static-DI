module di.bench.compile99.node1:impl;

import di.bench.compile99.node1;
import di;

namespace di::bench::compile99 {

template<class Context>
int Node1::Node<Context>::impl(trait::Trait1::get) const
{
    return i;
}

}
