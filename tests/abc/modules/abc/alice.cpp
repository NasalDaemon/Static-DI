module;
#if !DI_IMPORT_STD
#include <cstdio>
#endif
module abc.alice;

#if DI_IMPORT_STD
import std;
#endif

void abc::Alice::NodeBase::onGraphConstructed()
{
    std::puts("Constructed Alice");
}

void abc::Alice::NodeBase::impl(trait::Visitable::count, int& counter)
{
    std::puts("trait::Visitable::count: Visited Alice");
    counter++;
}

int abc::Alice::NodeBase::impl(trait::Alice::get) const
{
    return alice;
}
void abc::Alice::NodeBase::impl(trait::Alice::set, int value)
{
    alice = value;
}
