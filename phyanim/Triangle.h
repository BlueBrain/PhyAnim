#ifndef __PHYANIM_TRIANGLE__
#define __PHYANIM_TRIANGLE__

#include "Edge.h"
#include "Primitive.h"

namespace phyanim
{
class Triangle;

typedef Triangle* TrianglePtr;

typedef std::vector<TrianglePtr> Triangles;

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

typedef std::
    unordered_set<TrianglePtr, TrianglePointerHash, TrianglePointerEqual>
        UniqueTriangles;

class Triangle : public Primitive
{
public:
    Triangle(Node* n0_, Node* n1_, Node* n2_);

    virtual ~Triangle();

    Nodes nodes() const;

    Edges edges() const;

    double area() const;

    Vec3 normal() const;

    void sortedIds(unsigned int& id0_,
                   unsigned int& id1_,
                   unsigned int& id2_) const;

public:
    Node* node0;

    Node* node1;

    Node* node2;
};

typedef std::pair<TrianglePtr, TrianglePtr> TrianglePair;

typedef std::vector<TrianglePair> TrianglePairs;

}  // namespace phyanim

#endif
