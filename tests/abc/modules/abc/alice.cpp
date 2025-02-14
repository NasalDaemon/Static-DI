module;
#if !DI_STD_MODULE
#include <cstdio>
#endif
module abc.alice;

DI_IMPORT_STD;

void abc::Alice::NodeBase::onGraphConstructed()
{
    std::puts("Constructed Alice");
}

int abc::Alice::NodeBase::apply(trait::Alice::get) const
{
    return alice;
}
void abc::Alice::NodeBase::apply(trait::Alice::set, int value)
{
    alice = value;
}
