#pragma once

#include "Vertex.h"

namespace jecpp {

struct Edge {
    int weight;
    Vertex start;
    Vertex end;
};

} // namespace jecpp
