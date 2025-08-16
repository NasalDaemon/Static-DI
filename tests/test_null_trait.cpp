#include <doctest/doctest.h>

#include "di/macros.hpp"


import di.tests.null_trait;
import di;

namespace di::tests::null_trait {

/* di-embed-begin
export module di.tests.null_trait;

namespace di::tests::null_trait {

cluster Cluster [R = Root]
{
    node1 = R::Node
    node2 = R::Node

    [~] node1, node2
}

}

di-embed-end */

struct Root
{
    struct Node : di::Node
    {
        using Traits = di::NoTraits<Node>;

        int doSomething(this auto const& self)
        {
            return self.getNode(di::noTrait<Node>)->i;
        }

        int i;
    };
};

TEST_CASE("di::NullTrait")
{
    di::Graph<Cluster, Root> graph{
        .node1{DI_EMPLACE(.i = 1)},
        .node2{DI_EMPLACE(.i = 42)},
    };

    CHECK(graph.node1->doSomething() == 42);
    CHECK(graph.node2->doSomething() == 1);
}

}
