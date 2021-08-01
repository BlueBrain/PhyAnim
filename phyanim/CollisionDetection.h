#ifndef __PHYANIM_COLLISIONDETECTION__
#define __PHYANIM_COLLISIONDETECTION__

#include "Mesh.h"
#include "Tetrahedron.h"
#include "Triangle.h"

namespace phyanim
{
class CollisionDetection
{
public:
    static bool computeCollisions(HierarchicalAABBs& dynamics,
                                  double stiffness = 1000.0);

    static bool computeCollisions(HierarchicalAABBs& dynamics,
                                  HierarchicalAABBs& statics,
                                  double stiffness = 1000.0);

    static void computeCollisions(HierarchicalAABBs& dynamics,
                                  const AxisAlignedBoundingBox& aabb,
                                  double stiffness = 1000.0);

protected:
    static bool _checkMeshesCollision(HierarchicalAABBPtr haabb0,
                                      HierarchicalAABBPtr haabb1,
                                      double stiffness);

    static bool _checkTrianglesCollision(TrianglePtr t0,
                                         TrianglePtr t1,
                                         double stiffness);

    static void _checkAndSetForce(NodePtr node,
                                  Vec3 normal,
                                  double dist,
                                  double stiffness);
};

}  // namespace phyanim

#endif
