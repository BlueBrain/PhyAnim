#ifndef __PHYANIM_HIERARCHICAL_AABB__
#define __PHYANIM_HIERARCHICAL_AABB__

#include "AxisAlignedBoundingBox.h"
#include "Edge.h"

namespace phyanim
{
namespace geometry
{
class HierarchicalAABB;

typedef HierarchicalAABB* HierarchicalAABBPtr;

typedef std::vector<HierarchicalAABBPtr> HierarchicalAABBs;

class HierarchicalAABB : public AxisAlignedBoundingBox
{
public:
    HierarchicalAABB();

    HierarchicalAABB(Primitives& primitives, uint64_t cellSize = 10);

    HierarchicalAABB(Edges& edges, uint64_t cellSize = 10);

    ~HierarchicalAABB();

    void update();

    Nodes outterNodes(const AxisAlignedBoundingBox& axisAlignedBoundingBox);

    Primitives insidePrimitives(
        const AxisAlignedBoundingBox& axisAlignedBoundingBox);
    Primitives collidingPrimitives(
        const AxisAlignedBoundingBox& axisAlignedBoundingBox);
    PrimitivePairs collidingPrimitives(HierarchicalAABBPtr hierarchicalAABB);

    Edges insideEdges(const AxisAlignedBoundingBox& axisAlignedBoundingBox);
    Edges collidingEdges(const AxisAlignedBoundingBox& axisAlignedBoundingBox);

protected:
    void _update();

    void _divide(Primitives& primitives, uint64_t cellSize);

    void _outterNodes(const AxisAlignedBoundingBox& aabb, Nodes& nodes);

    void _insidePrimitives(const AxisAlignedBoundingBox& axisAlignedBoundingBox,
                           Primitives& primitives);

    void _collidingPrimitives(
        const AxisAlignedBoundingBox& axisAlignedBoundingBox,
        Primitives& primitives);

    void _collidingPrimitives(HierarchicalAABBPtr aabb0,
                              HierarchicalAABBPtr aabb1,
                              PrimitivePairs& primitivePairs);

protected:
    Primitives _primitives;

    HierarchicalAABBPtr _child0;

    HierarchicalAABBPtr _child1;
};

}  // namespace geometry
}  // namespace phyanim

#endif  // __PHYANIM_HIERARCHICAL_AABB__