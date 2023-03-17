#ifndef __EXAMPLES_COLLISION_SOLVER_H
#define __EXAMPLES_COLLISION_SOLVER_H

// #include <spatial_index/multi_index.hpp>

#include <Phyanim.h>

#include <chrono>

namespace examples
{

class CollisionSolver
{
public:
    CollisionSolver(double dt)
    {
        _system = new phyanim::ExplicitMassSpringSystem(dt);
        _system->gravity = false;
        _system->inertia = false;
    };

    ~CollisionSolver(){};

    uint32_t solveCollisions(phyanim::HierarchicalAABBs& aabbs,
                             std::vector<phyanim::Edges>& edgesSet,
                             std::vector<phyanim::Nodes>& nodesSet,
                             phyanim::AxisAlignedBoundingBox& limits,
                             uint32_t& totalIters)
    {
        phyanim::Edges edges;
        for (uint32_t i = 0; i < edgesSet.size(); ++i)
            edges.insert(edges.end(), edgesSet[i].begin(), edgesSet[i].end());
        phyanim::Nodes nodes = phyanim::uniqueNodes(edges);

        std::cout << "Simulation with " << edges.size() << " springs and "
                  << nodes.size() << " nodes" << std::endl;

        auto startTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime;
        uint32_t collisions = 1;
        double ks = 1000.0;
        double ksLimit = 0.01;
        uint32_t numIters = 1000;

        while (collisions > 0)
        {
            for (uint32_t iter = 0; iter < numIters; ++iter)
            {
                collisions =
                    solve(aabbs, edgesSet, nodesSet, limits, ks, 100.0, 0.0);
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
            if (ks < 0.01) break;
        }

        elapsedTime = std::chrono::steady_clock::now() - startTime;
        std::cout << "Iter: " << totalIters << "  Collisions: " << collisions
                  << "  Stiffness: " << ks << "  Time: " << elapsedTime.count()
                  << " seconds." << std::endl;

        return collisions;
    };

    uint32_t solve(phyanim::HierarchicalAABBs& aabbs,
                   std::vector<phyanim::Edges>& edgesSet,
                   std::vector<phyanim::Nodes>& nodesSet,
                   phyanim::AxisAlignedBoundingBox& limits,
                   double ks,
                   double ksc,
                   double kd)
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
            phyanim::CollisionDetection::computeCollisions(aabbs, ksc);
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

    uint32_t solve(phyanim::HierarchicalAABBs& aabbs,
                   phyanim::Edges& edges,
                   phyanim::Nodes& nodes,
                   phyanim::AxisAlignedBoundingBox& limits,
                   double ks,
                   double ksc,
                   double kd)
    {
        clearCollision(nodes);
        clearForce(nodes);

        uint32_t collisions =
            phyanim::CollisionDetection::computeCollisions(aabbs, ksc);

        if (collisions == 0) return 0;

        _system->step(nodes, edges, limits, ks, kd);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < aabbs.size(); ++i)
        {
            aabbs[i]->update();
        }

        return collisions;
    };

private:
    phyanim::ExplicitMassSpringSystem* _system;
};

}  // namespace examples

#endif
