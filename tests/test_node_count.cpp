import di.tests.count;
import di;

/*
di-embed-begin

export module di.tests.count;

namespace di::tests::count {

cluster InnerUnary [R = Root]
{
    c = R::Node
}

cluster InnerBinary [R = Root]
{
    d = R::Node
    e = R::Node
}

cluster OuterCluster [R = Root]
{
    a = R::Node
    b = R::Node
    innerUnary = InnerUnary
    innerBinary = InnerBinary
}

}

di-embed-end
*/

namespace di::tests::count {

struct Node : di::Node
{
    using Traits = di::Traits<Node>;
};
struct Root
{
    using Node = count::Node;
};

using Graph = di::Graph<OuterCluster, Root>;

static_assert(di::IsUnary<decltype(Graph::a)>);
static_assert(di::IsUnary<decltype(Graph::innerUnary)>);
static_assert(di::IsUnary<decltype(Graph::innerUnary.c)>);
static_assert(di::IsUnary<decltype(Graph::innerBinary.d)>);
static_assert(di::IsUnary<decltype(Graph::innerBinary.e)>);
static_assert(not di::IsUnary<decltype(Graph::innerBinary)>);
static_assert(not di::IsUnary<Graph>);

}
