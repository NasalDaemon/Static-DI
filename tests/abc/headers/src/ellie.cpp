#include "abc/ellie.hpp"

void abc::Ellie::onGraphConstructed() { std::puts("Constructed Ellie"); }

int abc::Ellie::apply(trait::Ellie::get) const { return value; }
