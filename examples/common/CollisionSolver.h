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
    CollisionSolver(double dt) : _dt(dt)
    {
        _system = new phyanim::ExplicitMassSpringSystem(_dt);
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
                    anim(aabbs, edgesSet, nodesSet, limits, ks, 100.0, 0.0);
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
            if (ks < 0.000001) break;
        }

        elapsedTime = std::chrono::steady_clock::now() - startTime;
        std::cout << "Iter: " << totalIters << "  Collisions: " << collisions
                  << "  Stiffness: " << ks << "  Time: " << elapsedTime.count()
                  << " seconds." << std::endl;

        return collisions;
    };

    uint32_t anim(phyanim::HierarchicalAABBs& aabbs,
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

    uint32_t solveSomasCollisions(phyanim::HierarchicalAABBs& aabbs,
                                  std::vector<phyanim::Edges>& edgesSet,
                                  std::vector<phyanim::Nodes>& nodesSet)
    {
        uint32_t numCollisions = 1;
        uint32_t iter = 0;
        double ksc = 1000.0;

        auto startTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime;

        while (numCollisions > 0)
        {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
            for (uint32_t i = 0; i < nodesSet.size(); ++i)
            {
                phyanim::clearCollision(nodesSet[i]);
                phyanim::clearForce(nodesSet[i]);
            }

            numCollisions =
                phyanim::CollisionDetection::computeCollisions(aabbs, ksc);

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

    void animSomas(phyanim::HierarchicalAABBs& aabbs,
                   std::vector<phyanim::Nodes>& nodesSet)
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < nodesSet.size(); ++i)
        {
            phyanim::Vec3 force = phyanim::Vec3::Zero();
            for (auto node : nodesSet[i])
            {
                force += node->force;
            }

            phyanim::Vec3 displace = force * _dt * _dt;

            for (auto node : nodesSet[i])
            {
                node->position = node->position + displace;
            }

            aabbs[i]->update();
        }
    }

private:
    phyanim::ExplicitMassSpringSystem* _system;

    double _dt;
};

}  // namespace examples

#endif
