#ifndef __EXAMPLES_COLLISION_SOLVER_H
#define __EXAMPLES_COLLISION_SOLVER_H

// #include <spatial_index/multi_index.hpp>

#include <Phyanim.h>

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
            // clearVelocityIfNoColl(nodesSet[i]);
            _system->step(nodesSet[i], edgesSet[i], limits, ks, kd);
            aabbs[i]->update();
        }
        return collisions;
    };

    bool solve(phyanim::HierarchicalAABBs& aabbs,
               phyanim::Edges& edges,
               phyanim::Nodes& nodes,
               phyanim::AxisAlignedBoundingBox& limits,
               double ks,
               double ksc,
               double kd)
    {
        uint32_t size = aabbs.size();
        clearCollision(nodes);
        clearForce(nodes);

        if (!phyanim::CollisionDetection::computeCollisions(aabbs, ksc))
            return true;

        std::cout << "#" << std::flush;

        // clearVelocityIfNoColl(nodes);
        _system->step(nodes, edges, limits, ks, kd);
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < size; ++i)
        {
            aabbs[i]->update();
        }

        return false;
    };

private:
    phyanim::ExplicitMassSpringSystem* _system;
};

}  // namespace examples

#endif
