#include "ExplicitMassSpringSystem.h"

#include <iostream>

namespace phyanim
{
namespace anim
{
ExplicitMassSpringSystem::ExplicitMassSpringSystem(float dt) : AnimSystem(dt) {}

ExplicitMassSpringSystem::~ExplicitMassSpringSystem() {}

void ExplicitMassSpringSystem::step(geometry::Nodes& nodes,
                                    geometry::Edges& edges,
                                    geometry::AxisAlignedBoundingBox& limits,
                                    float ks,
                                    float kd)
{
    _addGravity(nodes);

    for (uint32_t i = 0; i < edges.size(); ++i)
    {
        auto edge = edges[i];

        float l = glm::distance(edge->node1->position, edge->node0->position);
        geometry::Vec3 d = edge->node1->position - edge->node0->position;
        float r = edge->resLength;
        // Vec3 v = edge->node1->velocity - edge->node0->velocity;
        geometry::Vec3 f0 = geometry::Vec3();
        if (l > THRESHOLD && r > THRESHOLD) f0 = (d * ks * (l / r - 1.0f)) / l;

        // f0 = (ks * (l / r - 1.0) + kd * (v.dot(d) / (l * r))) * d / l;

        edge->node0->force += f0;
        edge->node1->force += -f0;
    }

    _update(nodes);

    // _updateIfCollide(nodes);

    limits.delimit(nodes);

    // limits.delimitIfCollide(nodes);
}

void ExplicitMassSpringSystem::_step(geometry::Mesh* mesh)
{
    auto ks = mesh->stiffness;
    auto kd = mesh->damping;

    for (auto edge : mesh->edges)
    {
        geometry::Vec3 d = edge->node1->position - edge->node0->position;
        float r = edge->resLength;
        float l = d.length();
        geometry::Vec3 v = edge->node1->velocity - edge->node0->velocity;
        geometry::Vec3 f0 = geometry::Vec3();
        if (l > THRESHOLD) l = THRESHOLD;

        f0 = d * (ks * (l / r - 1.0f) + kd * (glm::dot(v, d) / (l * r))) / l;
        edge->node0->force += f0;
        edge->node1->force += -f0;
    }

    for (unsigned int i = 0; i < mesh->nodes.size(); i++)
    {
        auto node = mesh->nodes[i];
        if (!node->fix && !node->isSoma)
        {
            geometry::Vec3 a = node->force / node->mass;
            geometry::Vec3 v = node->velocity + a * _dt;
            geometry::Vec3 x = node->position + v * _dt;
            node->velocity = v;
            node->position = x;
        }
    }
}

}  // namespace anim
}  // namespace phyanim
