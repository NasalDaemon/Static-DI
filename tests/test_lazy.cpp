#include <doctest/doctest.h>
#include "di/macros.hpp"

#if !DI_IMPORT_STD
#include <any>
#include <typeinfo>
#include <variant>
#include <vector>
#endif

import di;
#if DI_IMPORT_STD
import std;
#endif

namespace di::tests::lazy {

struct Trait : di::UnconstrainedTrait
{
    #define DI_METHODS_Trait(TAG) \
        TAG(get)

    DI_METHODS(Trait)
} inline constexpr trait{};

struct Node : di::Node
{
    using Traits = di::Traits<Node, Trait>;

    int apply(this auto& self, Trait::get)
    {
        return self.i + int(self.getNode(trait).get());
    }

    Node(int i = 88) : i(i) {}

    int i;
};

TEST_CASE("std::variant element offset")
{
    std::variant<int, double> v{9};
    CHECK(static_cast<void*>(&v) == static_cast<void*>(&std::get<int>(v)));
}

TEST_CASE("di::Lazy")
{
    di::test::Graph<di::Lazy<Node>> g{.node{33}};
    g.mocks->define([](Trait::get) { return 10; });
    auto g2 = std::move(g);
    auto g3 = g2;

    CHECK(g3.node->i == 33);
    CHECK(g3.node.asTrait(trait).get() == 43);
}

}
