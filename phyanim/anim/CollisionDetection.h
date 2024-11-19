/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __PHYANIM_COLLISIONDETECTION__
#define __PHYANIM_COLLISIONDETECTION__

#include "../geometry/Mesh.h"
#include "../geometry/Tetrahedron.h"
#include "../geometry/Triangle.h"

namespace phyanim
{
namespace anim
{
class CollisionDetection
{
public:
    static uint32_t computeCollisions(geometry::HierarchicalAABBs& aabbs,
                                      float stiffness,
                                      float threshold = 0.1f);

    static uint32_t computeSelfCollisions(geometry::HierarchicalAABBs& aabbs,
                                          float stiffness,
                                          float threshold = 0.1f);

    static uint32_t computeCollisions(geometry::HierarchicalAABBPtr aabb,
                                      float stiffness,
                                      float threshold = 0.1f);

    static bool computeCollisions(geometry::Meshes& meshes,
                                  float stiffness,
                                  float threshold = 0.1f);

    static void computeCollisions(geometry::HierarchicalAABBs& aabbs,
                                  const geometry::AxisAlignedBoundingBox& aabb);

    static void computeCollisions(geometry::Meshes& meshes,
                                  const geometry::AxisAlignedBoundingBox& aabb);

    static geometry::AxisAlignedBoundingBoxes collisionBoundingBoxes(
        geometry::HierarchicalAABBs& aabbs,
        float sizeFactor = 1.0);

    static geometry::AxisAlignedBoundingBoxes collisionBoundingBoxes(
        geometry::Meshes& meshes,
        float sizeFactor = 1.0);

protected:
    static uint32_t _computeCollision(geometry::HierarchicalAABBPtr aabb0,
                                      geometry::HierarchicalAABBPtr aabb1,
                                      float stiffness,
                                      float threshold);

    static bool _checkCollision(geometry::PrimitivePtr p0,
                                geometry::PrimitivePtr p1,
                                float stiffness,
                                float threshold,
                                bool setForces = true);

    static bool _checkCollision(geometry::TrianglePtr t0,
                                geometry::TrianglePtr t1,
                                float stiffness,
                                bool setForces = true);

    static bool _checkCollision(geometry::Edge* e0,
                                geometry::Edge* e1,
                                float stiffness,
                                float threshold,
                                bool setForces = true);

    static void _checkAndSetForce(geometry::NodePtr node,
                                  geometry::Vec3 normal,
                                  float dist,
                                  float stiffness);

    static void _mergeBoundingBoxes(geometry::AxisAlignedBoundingBoxes& aabbs);
};

}  // namespace anim
}  // namespace phyanim

#endif
