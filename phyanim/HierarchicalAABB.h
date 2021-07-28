#ifndef __PHYANIM_HIERARCHICAL_AABB__
#define __PHYANIM_HIERARCHICAL_AABB__

namespace phyanim
{
class AABBNode
{
public:
    AABBNode(void);

    AABBNode(AABBNode* other);

    ~AABBNode(void);

    void divide(void);

    void outterNodes(Nodes& nodes_, const AABB& aabb_);

    void trianglePairs(TrianglePairs& trianglePairs_,
                       AABBNode* node0_,
                       AABBNode* node1_);

    void update(void);

    AABB aabb;

    AABBNode* child0;

    AABBNode* child1;

    Nodes nodes;

    Triangles triangles;

    Tetrahedra tetrahedra;

protected:
    Vec3 _center(Triangle* triangle_);
};

class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox(void);

    AxisAlignedBoundingBox(Nodes& nodes_,
                           Triangles& triangles_,
                           Tetrahedra& tetrahedra_);

    virtual ~AxisAlignedBoundingBox(void);

    void generate(Nodes& nodes_,
                  Triangles& triangles_,
                  Tetrahedra& tetrahedra_);

    void update(void);

    Nodes outterNodes(const AABB& aabb_);

    TrianglePairs trianglePairs(AxisAlignedBoundingBox* other_);

    AABBNode* root;
};

}  // namespace phyanim

#endif  // __PHYANIM_HIERARCHICAL_AABB__