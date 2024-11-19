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

#ifndef __EXAMPLES_COLLISION_SOLVER_H
#define __EXAMPLES_COLLISION_SOLVER_H

// #include <spatial_index/multi_index.hpp>

#include <phyanim/Phyanim.h>

#include <chrono>

using namespace phyanim;

namespace examples
{
class CollisionSolver
{
public:
    CollisionSolver(float dt) : _dt(dt)
    {
        _system = new anim::ExplicitMassSpringSystem(_dt);
        _system->gravity = false;
        _system->inertia = false;
    };

    ~CollisionSolver(){};

    uint32_t solveCollisions(geometry::HierarchicalAABBs& aabbs,
                             std::vector<geometry::Edges>& edgesSet,
                             std::vector<geometry::Nodes>& nodesSet,
                             geometry::AxisAlignedBoundingBox& limits,
                             uint32_t& totalIters,
                             float threshold)
    {
        geometry::Edges edges;
        for (uint32_t i = 0; i < edgesSet.size(); ++i)
            edges.insert(edges.end(), edgesSet[i].begin(), edgesSet[i].end());
        geometry::Nodes nodes = geometry::uniqueNodes(edges);

        std::cout << "Simulation with " << edges.size() << " springs and "
                  << nodes.size() << " nodes" << std::endl;

        auto startTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsedTime;
        uint32_t collisions = 1;
        float ks = 1000.0f;
        float ksc = 100.0f;
        float ksLimit = 0.0001f;
        uint32_t numIters = 1000;

        while (collisions > 0)
        {
            for (uint32_t iter = 0; iter < numIters; ++iter)
            {
                collisions = anim(aabbs, edgesSet, nodesSet, limits, ks, ksc,
                                  0.0, threshold);
                if (totalIters % 100 == 0)
                {
                    elapsedTime = std::chrono::steady_clock::now() - startTime;
                    std::cout << "Iter: " << totalIters
                              << "  Collisions: " << collisions
                              << "  Stiffness: " << ks
                              << "  Time: " << elapsedTime.count()
                              << " seconds." << std::endl;
                }
                totalIters++;
                if (collisions == 0) break;
            }

            ks *= 0.75;
            numIters *= 0.75;
            if (numIters < 100) numIters = 100;
            if (ks < ksLimit) break;
        }

        elapsedTime = std::chrono::steady_clock::now() - startTime;
        std::cout << "Final result -> Iter: " << totalIters
                  << "  Collisions: " << collisions << "  Stiffness: " << ks
                  << "  Time: " << elapsedTime.count() << " seconds."
                  << std::endl;

        return collisions;
    };

    uint32_t anim(geometry::HierarchicalAABBs& aabbs,
                  std::vector<geometry::Edges>& edgesSet,
                  std::vector<geometry::Nodes>& nodesSet,
                  geometry::AxisAlignedBoundingBox& limits,
                  float ks,
                  float ksc,
                  float kd,
                  float threshold)
    {
        uint32_t size = edgesSet.size();
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < size; ++i)
        {
            clearForce(nodesSet[i]);
            clearCollision(nodesSet[i]);
        }

        uint32_t collisions =
            anim::CollisionDetection::computeCollisions(aabbs, ksc, threshold);
        if (collisions == 0) return 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < size; ++i)
        {
            _system->step(nodesSet[i], edgesSet[i], limits, ks, kd);
            aabbs[i]->update();
        }
        return collisions;
    };

    uint32_t solveSomasCollisions(geometry::HierarchicalAABBs& aabbs,
                                  std::vector<geometry::Edges>& edgesSet,
                                  std::vector<geometry::Nodes>& nodesSet,
                                  float ksc)
    {
        uint32_t numCollisions = 1;
        uint32_t iter = 0;

        auto startTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsedTime;

        while (numCollisions > 0)
        {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
            for (uint32_t i = 0; i < nodesSet.size(); ++i)
            {
                geometry::clearCollision(nodesSet[i]);
                geometry::clearForce(nodesSet[i]);
            }

            numCollisions =
                anim::CollisionDetection::computeCollisions(aabbs, ksc);

            animSomas(aabbs, nodesSet);

            if (iter % 100 == 0 | numCollisions == 0)
            {
                elapsedTime = std::chrono::steady_clock::now() - startTime;
                std::cout << "Iter: " << iter
                          << "  Collisions: " << numCollisions
                          << "  Time: " << elapsedTime.count() << " seconds."
                          << std::endl;
            }
            ++iter;
        }
        return iter;
    }

    void animSomas(geometry::HierarchicalAABBs& aabbs,
                   std::vector<geometry::Nodes>& nodesSet)
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < nodesSet.size(); ++i)
        {
            geometry::Vec3 force = geometry::Vec3();
            for (auto node : nodesSet[i])
            {
                force += node->force;
            }

            geometry::Vec3 displace = force * _dt;

            for (auto node : nodesSet[i])
            {
                node->position = node->position + displace;
            }

            aabbs[i]->update();
        }
    }

private:
    anim::ExplicitMassSpringSystem* _system;

    float _dt;
};

}  // namespace examples

#endif
