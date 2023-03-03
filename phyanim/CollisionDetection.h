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
    static uint32_t computeCollisions(HierarchicalAABBs& aabbs,
                                      double stiffness);

    static uint32_t computeSelfCollisions(HierarchicalAABBs& aabbs,
                                          double stiffness);

    static uint32_t computeCollisions(HierarchicalAABBPtr aabb,
                                      double stiffness);

    static bool computeCollisions(Meshes& meshes, double stiffness);

    static void computeCollisions(HierarchicalAABBs& aabbs,
                                  const AxisAlignedBoundingBox& aabb);

    static void computeCollisions(Meshes& meshes,
                                  const AxisAlignedBoundingBox& aabb);

    static AxisAlignedBoundingBoxes collisionBoundingBoxes(
        HierarchicalAABBs& aabbs,
        double sizeFactor = 1.0);

    static AxisAlignedBoundingBoxes collisionBoundingBoxes(
        Meshes& meshes,
        double sizeFactor = 1.0);

protected:
    static uint32_t _computeCollision(HierarchicalAABBPtr aabb0,
                                      HierarchicalAABBPtr aabb1,
                                      double stiffness);

    static bool _checkCollision(PrimitivePtr p0,
                                PrimitivePtr p1,
                                double stiffness,
                                bool setForces = true);

    static bool _checkCollision(TrianglePtr t0,
                                TrianglePtr t1,
                                double stiffness,
                                bool setForces = true);

    static bool _checkCollision(Edge* e0,
                                Edge* e1,
                                double stiffness,
                                bool setForces = true);

    static void _checkAndSetForce(NodePtr node,
                                  Vec3 normal,
                                  double dist,
                                  double stiffness);

    static void _mergeBoundingBoxes(AxisAlignedBoundingBoxes& aabbs);
};

}  // namespace phyanim

#endif
