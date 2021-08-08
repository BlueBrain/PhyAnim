#ifndef __PHYANIM_COLLISIONDETECTION__
#define __PHYANIM_COLLISIONDETECTION__

#include "DrawableMesh.h"
#include "Tetrahedron.h"
#include "Triangle.h"

namespace phyanim
{
class CollisionDetection
{
public:
    static bool computeCollisions(Meshes& meshes,
                                  double stiffness = 1000.0,
                                  bool setColor = false);

    static bool computeCollisions(Meshes& dynamics,
                                  Meshes& statics,
                                  double stiffness = 1000.0,
                                  bool setColor = false);

    static void computeCollisions(Meshes& meshes,
                                  const AxisAlignedBoundingBox& aabb);

    static AxisAlignedBoundingBoxes collisionBoundingBoxes(
        Meshes& meshes,
        double sizeFactor = 10);

protected:
    static bool _checkMeshesCollision(MeshPtr mesh0,
                                      MeshPtr mesh1,
                                      double stiffness);

    static bool _checkMeshesCollisionAndSetColor(MeshPtr mesh0,
                                                 MeshPtr mesh1,
                                                 double stiffness);

    static bool _checkTrianglesCollision(TrianglePtr t0,
                                         TrianglePtr t1,
                                         double stiffness,
                                         bool setForces = true);

    static void _checkAndSetForce(NodePtr node,
                                  Vec3 normal,
                                  double dist,
                                  double stiffness);

    static void _mergeBoundingBoxes(AxisAlignedBoundingBoxes& aabbs);

    static const Vec3 _color;
    static const Vec3 _collisionColor;
};

}  // namespace phyanim

#endif
