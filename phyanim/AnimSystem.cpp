#include <iostream>

#include <AnimSystem.h>

namespace phyanim {

AnimSystem::AnimSystem(double dt) : gravity(true), _dt(dt) {}

AnimSystem::~AnimSystem() {}

void AnimSystem::step()
{    
    if (gravity)
    {
        Vec3 g = Vec3::Zero();
        g.y() = -9.8f;
        for (auto mesh: _meshes)
        {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
            for (unsigned int i = 0; i<mesh->nodes.size(); i++)
            {
                auto node = mesh->nodes[i];
                node->force += g*node->mass;
            }
        }   
        _step();
    }
}

void AnimSystem::addMesh(Mesh* mesh)
{
    _meshes.push_back(mesh);   
}

void AnimSystem::addMeshes(Meshes meshes)
{
    clear();
    for (auto mesh: meshes)
    {
        addMesh(mesh);
    }
}

void AnimSystem::clear()
{
    _meshes.clear();
}

}
