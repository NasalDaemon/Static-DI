module;
#if !DI_IMPORT_STD
#include <cstdio>
#endif
module abc.ellie;

#if DI_IMPORT_STD
import std;
#endif

void abc::Ellie::onGraphConstructed() { std::puts("Constructed Ellie"); }

int abc::Ellie::apply(trait::Ellie::get) const { return value; }
