#pragma once

#include "abc/graph.hxx"

#include "di/graph.hpp"

namespace abc {

struct GraphWrapper
{
    di::Graph<abc::AliceBob> graph;
};

}
