#ifndef __PHYANIM_EDGE__
#define __PHYANIM_EDGE__

#include <Node.h>

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

class Edge
{
public:
    Edge(Node* node0_, Node* node1_);

    virtual ~Edge(void);

    bool operator==(const Edge& edge_) const;

    Node* node0;

    Node* node1;

    double resLength;
};

}  // namespace phyanim

#endif
