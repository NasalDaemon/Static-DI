#include "abc/graph_wrapper.hpp"

using namespace abc;

inline int test()
{
    abc::GraphWrapper wrapper;
    auto& g = wrapper.graph;
    auto aliceWrite = g.asTrait(trait::alice);
    aliceWrite.set(11);

    using AliceWrite = decltype(aliceWrite);
    static_assert(std::is_same_v<int, AliceWrite::Types::AliceType>);
    static_assert(aliceWrite.isTrait(trait::aliceWrite));
    static_assert(aliceWrite.isTrait(trait::aliceRead));

    if (0 != g.charlie.asTrait(trait::charlie).get() + g.charlie.charlie.asTrait(trait::charlie2).get())
        return 1;
    if (g.charlie.charlie.asTrait(trait::charlie3).get() != 15)
        return 1;

    if (11 != g.charlie.charlie.asTrait(trait::aliceRead).get())
        return 1;

    return 0;
}

int main()
{
    return test();
}
