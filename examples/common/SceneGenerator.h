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

#ifndef __EXAMPLES_SCENE_GENERATOR__
#define __EXAMPLES_SCENE_GENERATOR__

#include <phyanim/Phyanim.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace phyanim;

namespace examples
{
class SceneGenerator
{
public:
    static geometry::Meshes generate(geometry::Meshes meshes,
                                     uint32_t numOutMeshes,
                                     geometry::AxisAlignedBoundingBox aabb,
                                     float bbFactor = 15.0,
                                     uint32_t maxCollisions = 4,
                                     float maxCollisionRadius = 4.0)
    {
        std::srand(std::time(nullptr));
        geometry::Meshes sceneMeshes;
        uint32_t numMeshes = meshes.size();

        if (numMeshes > 0)
        {
            geometry::Vec3 range(aabb.upperLimit() - aabb.lowerLimit());

            std::cout << std::fixed << std::setprecision(2);
            float progress = 0.0;
            std::cout << "Generating process " << progress << "%" << std::flush;

            uint32_t numCollisions = 0;
            for (uint32_t i = 0; i < numOutMeshes; ++i)
            {
                uint32_t meshId = std::rand() % numMeshes;
                auto mesh = meshes[meshId]->copy();
                sceneMeshes.push_back(mesh);
                mesh->boundingBox =
                    new geometry::HierarchicalAABB(mesh->surfaceTriangles);
                bool rightPos = false;

                while (!rightPos)
                {
                    geometry::Vec3 pos;
                    pos.x = (float)std::rand() / RAND_MAX * range.x;
                    pos.y = (float)std::rand() / RAND_MAX * range.y;
                    pos.z = (float)std::rand() / RAND_MAX * range.z;

                    float alpha = (float)std::rand() / RAND_MAX * 2 * M_PI;
                    float beta = (float)std::rand() / RAND_MAX * 2 * M_PI;
                    float gamma = (float)std::rand() / RAND_MAX * 2 * M_PI;

                    float cosX = std::cos(alpha);
                    float sinX = std::sin(alpha);
                    float cosY = std::cos(beta);
                    float sinY = std::sin(beta);
                    float cosZ = std::cos(gamma);
                    float sinZ = std::sin(gamma);
                    geometry::Mat3 rot;
                    geometry::Mat3 xRot = {1,     0, 0,    0,   cosX,
                                           -sinX, 0, sinX, cosX};
                    geometry::Mat3 yRot = {cosY, 0,     sinY, 0,   1,
                                           0,    -sinY, 0,    cosY};
                    geometry::Mat3 zRot = {cosZ, -sinZ, 0, sinZ, cosZ,
                                           0,    0,     0, 1};
                    rot = xRot * yRot * zRot;

                    geometry::Vec3 center = mesh->boundingBox->center();
                    for (auto node : mesh->nodes)
                    {
                        geometry::Vec3 position = node->position - center;
                        position = rot * position;
                        position += pos;
                        node->initPosition = position;
                        node->position = position;
                        node->normal = rot * node->normal;
                    }
                    mesh->boundingBox->update();

                    auto aabbs =
                        anim::CollisionDetection::collisionBoundingBoxes(
                            sceneMeshes, bbFactor);

                    rightPos =
                        ((aabbs.size() - numCollisions) <= maxCollisions);
                    if (rightPos)
                    {
                        for (auto aabb : aabbs)
                            rightPos &= aabb->radius() < maxCollisionRadius;
                    }

                    if (rightPos) numCollisions = aabbs.size();
                }
                progress += 100.0 / numOutMeshes;
                std::cout << "\rGenerating process " << progress << "%"
                          << std::flush;
            }
            std::cout << std::endl;
            std::cout << "Number of collisions: " << numCollisions << std::endl;
        }
        return sceneMeshes;
    };

private:
    SceneGenerator(){};
};

}  // namespace examples

#endif