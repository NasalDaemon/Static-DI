#include "di/macros.hpp"
#if !DI_STD_MODULE
#include <type_traits>
#endif

import di;
import abc.graph;
DI_IMPORT_STD;

using namespace abc;

inline int test()
{
    auto g = DI_CONSTRUCT(di::Graph<AliceBob>{.ellie{101}});
    auto aliceWrite = g.asTrait(trait::alice);
    aliceWrite.set(11);

    using AliceWrite = decltype(aliceWrite);
    static_assert(std::is_same_v<int, AliceWrite::Types::AliceType>);
    static_assert(aliceWrite.isTrait(trait::aliceWrite));
    static_assert(aliceWrite.isTrait(trait::aliceRead));

    if (0 != g.charlie.asTrait(trait::charlie).get() + g.charlie.charlie.asTrait(trait::charlie2).get())
        return 1;
    if (15 != g.charlie.charlie.asTrait(trait::charlie3).get())
        return 1;

    if (64 != g.bob.asTrait(trait::bob).get())
        return 1;
    if (99 != g.bob.asTrait(trait::charlie).get())
        return 1;

    if (11 != g.charlie.charlie.asTrait(trait::aliceRead).get())
        return 1;

    if (101 != g.ellie.asTrait(trait::ellie).get())
        return 1;

    if (99 != g.ellie.asTrait(trait::charlie).get())
        return 1;
    if (99 != g.ellie.asTrait(trait::charlie2).get())
        return 1;

    auto charlie2get = g.ellie.asTrait(trait::charlie2).get(di::asFunctor);
    if (99 != charlie2get())
        return 1;

    if (101 != g.ellie->value)
        return 1;

    return 0;
}

int main()
{
    return test();
}
