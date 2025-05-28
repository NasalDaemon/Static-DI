#include <doctest/doctest.h>

import di.tests.embedded;
import di;

#if 0
// di-embed-begin
export module di.tests.embedded;

trait di::tests::trait::Embedded
{
    get() const
}
// di-embed-end

random code between two sections will be ignored

// di-embed-begin
cluster di::tests::EmbeddedCluster [R = Root]
{
    embedded = R::Embedded

    [trait::Embedded]
    .. --> embedded
}
// di-embed-end
#endif

namespace di::tests {

struct EmbeddedNode : di::Node
{
    using Traits = di::Traits<EmbeddedNode, trait::Embedded>;

    int apply(trait::Embedded::get) const { return 42; }
};

}

using namespace di::tests;

TEST_CASE("embedded")
{
    struct Root
    {
        using Embedded = EmbeddedNode;
    };
    di::Graph<EmbeddedCluster, Root> g{};

    CHECK(g.asTrait(trait::embedded).get() == 42);
}
