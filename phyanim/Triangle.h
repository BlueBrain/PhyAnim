#ifndef __PHYANIM_TRIANGLE__
#define __PHYANIM_TRIANGLE__

#include "Edge.h"

namespace phyanim
{
class Triangle;

typedef std::vector<Triangle*> Triangles;

struct TrianglePointerHash
{
public:
    size_t operator()(const Triangle* triangle_) const;
};

struct TrianglePointerEqual
{
public:
    bool operator()(const Triangle* triangle0_,
                    const Triangle* triangle_) const;
};

typedef std::unordered_set<Triangle*, TrianglePointerHash, TrianglePointerEqual>
    UniqueTriangles;

class Triangle
{
public:
    Triangle(Node* n0_, Node* n1_, Node* n2_);

    virtual ~Triangle(void);

    Node* node0;

    Node* node1;

    Node* node2;

    Edges edges(void);

    double area(void);

    void sortedIds(unsigned int& id0_,
                   unsigned int& id1_,
                   unsigned int& id2_) const;
};

typedef std::pair<Triangle*, Triangle*> TrianglePair;

typedef std::vector<TrianglePair> TrianglePairs;

}  // namespace phyanim

#endif
