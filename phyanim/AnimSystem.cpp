#include <AnimSystem.h>

#include <iostream>

namespace phyanim
{
AnimSystem::AnimSystem(double dt) : gravity(true), _dt(dt) {}

AnimSystem::~AnimSystem() {}

void AnimSystem::step(Mesh* mesh)
{
    if (gravity)
    {
        Vec3 g = Vec3::Zero();
        g.y() = -9.8f;

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i = 0; i < mesh->nodes.size(); i++)
        {
            auto node = mesh->nodes[i];
            node->force += g * node->mass;
        }
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
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
            for (unsigned int i = 0; i < mesh->nodes.size(); i++)
            {
                auto node = mesh->nodes[i];
                node->force += g * node->mass;
            }
        }
    }
    for (auto mesh : meshes)
    {
        _step(mesh);
    }
}
void AnimSystem::preprocessMesh(Meshes meshes)
{
    for (auto mesh : meshes)
    {
        preprocessMesh(mesh);
    }
};

}  // namespace phyanim
