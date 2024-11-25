/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
