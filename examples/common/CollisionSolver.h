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
    };

    ~CollisionSolver(){};

    bool solve(phyanim::HierarchicalAABBs& aabbs,
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
            _system->clearCollision(edgesSet[i]);
            _system->clearForce(nodesSet[i]);
        }
        if (!phyanim::CollisionDetection::computeCollisions(aabbs, ksc))
            return true;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < size; ++i)
        {
            _system->step(nodesSet[i], edgesSet[i], limits, ks, kd);
            aabbs[i]->update();
        }
        return false;
    };

private:
    phyanim::ExplicitMassSpringSystem* _system;
};

}  // namespace examples

#endif
