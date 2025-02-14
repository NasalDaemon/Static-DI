#include "di/macros.hpp"

#include <doctest/doctest.h>

#if !DI_STD_MODULE
#include <type_traits>
#endif

import di;
import abc.graph;
DI_IMPORT_STD;

using namespace abc;

TEST_CASE("Test abc module")
{
    auto g = DI_CONSTRUCT(di::Graph<AliceBob>{.ellie{101}});
    auto aliceWrite = g.asTrait(trait::alice);
    aliceWrite.set(11);

    using AliceWrite = decltype(aliceWrite);
    static_assert(std::is_same_v<int, AliceWrite::Types::AliceType>);
    static_assert(aliceWrite.isTrait(trait::aliceWrite));
    static_assert(aliceWrite.isTrait(trait::aliceRead));

    auto charlie1plus2 = g.charlie.asTrait(trait::charlie).get() + g.charlie.charlie.asTrait(trait::charlie2).get();
    CHECK(0 == charlie1plus2);

    CHECK(15 == g.charlie.charlie.asTrait(trait::charlie3).get());

    CHECK(64 == g.bob.asTrait(trait::bob).get());

    CHECK(99 == g.bob.asTrait(trait::charlie).get());

    CHECK(11 == g.charlie.charlie.asTrait(trait::aliceRead).get());

    CHECK(101 == g.ellie.asTrait(trait::ellie).get());

    CHECK(99 == g.ellie.asTrait(trait::charlie).get());

    CHECK(99 == g.ellie.asTrait(trait::charlie2).get());

    auto charlie2get = g.ellie.asTrait(trait::charlie2).get(di::asFunctor);
    CHECK(99 == charlie2get());

    CHECK(101 == g.ellie->value);
}
