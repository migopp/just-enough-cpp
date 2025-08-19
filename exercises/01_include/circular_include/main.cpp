#include "Edge.h"
#include "Vertex.h"

int main()
{
    jecpp::Vertex v1 = { 1, new jecpp::Edge[1] };
    jecpp::Vertex v2 = { 0, new jecpp::Edge[1] };

    jecpp::Edge e = { 28, v1, v2 };
    v1.edgeList[0] = e;
    v2.edgeList[0] = e;

    return v1.edgeList[0].end.value;
}
