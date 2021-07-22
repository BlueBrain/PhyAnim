
#ifndef __PHYANIM_AXISALIGNEDBOUNDINGBOX__
#define __PHYANIM_AXISALIGNEDBOUNDINGBOX__

#include <BoundingVolume.h>
#include <Tetrahedron.h>
#include <Triangle.h>

namespace phyanim
{
typedef std::pair<Triangle*, Triangle*> TrianglePair;

typedef std::vector<TrianglePair> TrianglePairs;

class AABB
{
public:
    AABB(void);

    AABB(Vec3 lowerLimit_, Vec3 upperLimit_);

    AABB(const AABB& other_);

    ~AABB(void);

    bool collide(const AABB& other_) const;

    bool inside(const AABB& other_) const;

    bool inside(const Vec3& pos_) const;

    void update(const AABB& other_);

    void update(const Vec3& pos_);

    Vec3 center(void);

    Vec3 lowerLimit;

    Vec3 upperLimit;
};

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

#endif
