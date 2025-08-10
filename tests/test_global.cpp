#include <doctest/doctest.h>

#if !DI_IMPORT_STD
#include <print>
#include <string_view>
#endif

import di.tests.global;
import di;

namespace di::tests::global {

/* di-embed-begin
export module di.tests.global;

namespace di::tests::global {

trait trait::Log
{
    log(auto message) -> std::same_as<void> auto
}

cluster MainCluster [R = Root]
{
    node = R::Node
    cluster = InnerCluster

    [trait::Log]
    node --> *
}

cluster InnerCluster [R = Root]
{
    node = R::Node

    [trait::Log]
    node --> *
}

cluster GlobalCluster [R = Root]
{
    logger = R::Logger

    [trait::Log]
    .. --> logger
}

}

di-embed-end */

struct Root
{
    struct Node : di::Node
    {
        using Traits = di::Traits<Node>;
        using Depends = di::Depends<trait::Log, di::Global<trait::Log>>;
    };
    struct Logger : di::Node
    {
        using Traits = di::Traits<Logger, trait::Log>;

        void impl(trait::Log::log, std::string_view message)
        {
            std::println("Log: {}", message);
            count++;
        }

        int count = 0;
    };
};

TEST_CASE("di::Global")
{
    di::Graph<GlobalCluster, Root> global;

    di::GraphWithGlobal<MainCluster, GlobalCluster*, Root> graph{
        .global = &global,
        .main{
            .node{},
            .cluster{},
        }
    };
    CHECK(global.logger->count == 0);
    graph->node.getGlobal(trait::log).log("Hello, Global!");
    CHECK(global.logger->count == 1);
}

TEST_CASE("di::Global hosted locally")
{
    di::GraphWithGlobal<MainCluster, Root::Logger, Root> graph{
        .global{},
        .main{
            .node{},
            .cluster{},
        }
    };

    CHECK(graph.global->count == 0);

    // Explicit global resolution
    graph->node.getGlobal(trait::log).log("Hello, Global!");
    CHECK(graph.global->count == 1);

    // Redirected global resolution via *
    graph->node.getNode(trait::log).log("Hello, Global from InnerCluster!");
    CHECK(graph.global->count == 2);

    // Redirected global resolution via *
    graph->cluster.node.getNode(trait::log).log("Hello, Global from InnerCluster!");
    CHECK(graph.global->count == 3);
}

// TODO: Support di::Box?

}
