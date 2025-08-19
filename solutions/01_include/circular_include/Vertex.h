#pragma once

namespace jecpp {

struct Edge;

struct Vertex {
    int value;
    // Don't know how many elements to allocated with each vertex, so we just decay it to a pointer.
    Edge* edgeList;
};

} // namespace jecpp
