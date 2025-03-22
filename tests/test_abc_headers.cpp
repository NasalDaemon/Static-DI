#include "abc/graph_wrapper.hpp"

#include <doctest/doctest.h>

using namespace abc;

TEST_CASE("abc headers")
{
    abc::GraphWrapper wrapper;
    auto& g = wrapper.graph;
    auto aliceWrite = g.asTrait(trait::alice);
    aliceWrite.set(11);

    using AliceWrite = decltype(aliceWrite);
    static_assert(std::is_same_v<int, AliceWrite::Types::AliceType>);
    static_assert(aliceWrite.isTrait(trait::aliceWrite));
    static_assert(aliceWrite.isTrait(trait::aliceRead));

    auto charlie1plus2 = g.charlie.asTrait(trait::charlie).get() + g.charlie.charlie.asTrait(trait::charlie2).get();
    CHECK(0 == charlie1plus2);

    CHECK(15 == g.charlie.charlie.asTrait(trait::charlie3).get());

    CHECK(11 == g.charlie.charlie.asTrait(trait::aliceRead).get());
}
