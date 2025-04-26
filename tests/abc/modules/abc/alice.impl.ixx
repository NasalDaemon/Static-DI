module;
#if !DI_STD_MODULE
#include <cstdio>
#endif
module abc.alice:impl;

import abc.alice;
import abc.traits;
#if DI_STD_MODULE
import std;
#endif

#define ALICE \
    template<class Context> \
    auto abc::Alice::Node<Context>

ALICE::apply(trait::Bob::get) const -> int
{
    return getNode(trait::bob).get();
}
ALICE::apply(trait::Bob::set, int) -> void
{
}

ALICE::apply(trait::Charlie::get method) const -> int
{
    return getNode(traitOf(method)).get();
}
