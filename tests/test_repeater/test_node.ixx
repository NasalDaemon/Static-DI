export module di.tests.repeater.test_node;

import di.tests.repeater.traits;

import di;

export namespace di::tests::repeater {

struct TestNode : di::Node
{
    void impl(trait::Trait::function, int& i) const
    {
        i++;
    }
    void impl(this auto const& self, trait::Trait::defer, int& i)
    {
        self.getNode(trait::trait).function(i);
    }

    using Traits = di::Traits<TestNode, trait::Trait>;
};

} // namespace di::tests::repeater
