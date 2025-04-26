module;
#if !DI_STD_MODULE
#include <cstdio>
#endif
module abc.alice;

#if DI_STD_MODULE
import std;
#endif

void abc::Alice::NodeBase::onGraphConstructed()
{
    std::puts("Constructed Alice");
}

void abc::Alice::NodeBase::apply(trait::Visitable::count, int& counter)
{
    std::puts("trait::Visitable::count: Visited Alice");
    counter++;
}

int abc::Alice::NodeBase::apply(trait::Alice::get) const
{
    return alice;
}
void abc::Alice::NodeBase::apply(trait::Alice::set, int value)
{
    alice = value;
}
