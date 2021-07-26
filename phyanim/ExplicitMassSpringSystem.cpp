#include <ExplicitMassSpringSystem.h>

#include <iostream>

namespace phyanim
{
ExplicitMassSpringSystem::ExplicitMassSpringSystem(double dt) : AnimSystem(dt)
{
}

ExplicitMassSpringSystem::~ExplicitMassSpringSystem() {}

void ExplicitMassSpringSystem::_step(Mesh* mesh)
{
    auto ks = mesh->stiffness;
    auto kd = mesh->damping;

    for (auto edge : mesh->edges)
    {
        Vec3 d = edge->node1->position - edge->node0->position;
        double r = edge->resLength;
        double l = d.norm();
        Vec3 v = edge->node1->velocity - edge->node0->velocity;
        Vec3 f0 = Vec3::Zero();
        if (l > 0.0)
        {
            f0 = (ks * (l / r - 1.0) + kd * (v.dot(d) / (l * r))) * d / l;
        }
        edge->node0->force += f0;
        edge->node1->force += -f0;
    }

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < mesh->nodes.size(); i++)
    {
        auto node = mesh->nodes[i];
        if (!node->fixed)
        {
            Vec3 a = node->force / node->mass;
            Vec3 v = node->velocity + a * _dt;
            Vec3 x = node->position + v * _dt;
            node->velocity = v;
            node->position = x;
        }
    }
}

}  // namespace phyanim
