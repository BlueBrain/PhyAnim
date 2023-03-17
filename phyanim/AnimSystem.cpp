#include <AnimSystem.h>

#include <iostream>

namespace phyanim
{
AnimSystem::AnimSystem(double dt) : gravity(true), inertia(true), _dt(dt) {}

AnimSystem::~AnimSystem() {}

void AnimSystem::step(Mesh* mesh)
{
    if (gravity)
    {
        Vec3 g = Vec3::Zero();
        g.y() = -9.8f;

        for (unsigned int i = 0; i < mesh->nodes.size(); i++)
        {
            auto node = mesh->nodes[i];
            node->force += g * node->mass;
        }
    }
    for (unsigned int i = 0; i < mesh->nodes.size(); i++)
    {
        auto node = mesh->nodes[i];
        node->anim = true;
    }
    _step(mesh);
}
void AnimSystem::step(Meshes meshes)
{
    if (gravity)
    {
        Vec3 g = Vec3::Zero();
        g.y() = -9.8f;
        for (auto mesh : meshes)
        {
            for (unsigned int i = 0; i < mesh->nodes.size(); i++)
            {
                auto node = mesh->nodes[i];
                node->force += g * node->mass;
            }
        }
    }
    for (auto mesh : meshes)
    {
        for (unsigned int i = 0; i < mesh->nodes.size(); i++)
        {
            auto node = mesh->nodes[i];
            node->anim = true;
        }

        _step(mesh);
    }
}
void AnimSystem::_addGravity(Nodes& nodes)
{
    if (gravity)
    {
        uint32_t size = nodes.size();
        Vec3 g = Vec3(0, -9.8f, 0);
        for (uint32_t i = 0; i < size; ++i)
            nodes[i]->force += g * nodes[i]->mass;
    }
}

void AnimSystem::_update(Nodes& nodes)
{
    uint32_t size = nodes.size();

    for (uint32_t i = 0; i < size; ++i)
    {
        auto node = nodes[i];
        if (!node->fix && !node->isSoma)
        {
            Vec3 a = node->force / node->mass;
            Vec3 v = node->velocity + a * _dt;
            Vec3 x = node->position + v * _dt;
            node->position = x;
            if (inertia) node->velocity = v;
        }
    }
}

void AnimSystem::_updateIfCollide(Nodes& nodes)
{
    for (uint32_t i = 0; i < nodes.size(); ++i)
    {
        auto node = nodes[i];
        if (!node->fix && !node->isSoma && node->collide)
        {
            Vec3 a = node->force / node->mass;
            Vec3 v = node->velocity + a * _dt;
            Vec3 x = node->position + v * _dt;
            node->position = x;
            if (inertia) node->velocity = v;
        }
    }
}

void AnimSystem::preprocessMesh(Meshes meshes)
{
    for (auto mesh : meshes)
    {
        preprocessMesh(mesh);
    }
}

}  // namespace phyanim
