#ifndef __PHYANIM_EDGE__
#define __PHYANIM_EDGE__

#include <AxisAlignedBoundingBox.h>

#include <unordered_set>

namespace phyanim
{
class Edge;

typedef std::vector<Edge*> Edges;

struct EdgePointerHash
{
public:
    size_t operator()(const Edge* edge_) const;
};

struct EdgePointerEqual
{
public:
    bool operator()(const Edge* edge0_, const Edge* edge1_) const;
};

typedef std::unordered_set<Edge*, EdgePointerHash, EdgePointerEqual>
    UniqueEdges;

class Edge : public Primitive
{
public:
    Edge(Node* node0_, Node* node1_);

    virtual ~Edge(void);

    Nodes nodes() const
    {
        Nodes nodes(2);
        nodes[0] = node0;
        nodes[1] = node1;
        return nodes;
    };

    Edges edges() const
    {
        Edges edges(1);
        edges[0] = new Edge(node0, node1);
        return edges;
    };

    void computeLen();

    bool operator==(const Edge& edge_) const;

    Node* node0;

    Node* node1;

    double resLength;
};

void computeLens(Edges& edges);

Nodes uniqueNodes(Edges& edges);

void resample(Edges& edges);

void removeOutEdges(Edges& edges, AxisAlignedBoundingBox& limits);

}  // namespace phyanim

#endif
