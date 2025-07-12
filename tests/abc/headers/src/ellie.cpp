#include "abc/ellie.hpp"

void abc::Ellie::onGraphConstructed() { std::puts("Constructed Ellie"); }

int abc::Ellie::impl(trait::Ellie::get) const { return value; }
