module;
#if !DI_STD_MODULE
#include <cstdio>
#endif
module abc.ellie;

#if DI_STD_MODULE
import std;
#endif

void abc::Ellie::onGraphConstructed() { std::puts("Constructed Ellie"); }

int abc::Ellie::apply(trait::Ellie::get) const { return value; }
