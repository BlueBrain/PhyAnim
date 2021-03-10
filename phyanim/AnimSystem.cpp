#include <iostream>

#include <AnimSystem.h>

namespace phyanim {

AnimSystem::AnimSystem(double dt_, CollisionDetection* collDetector_)
    : gravity(true)
    , collisions(true)
    , _dt(dt_)
    , _collisionDetector(collDetector_) {
    if (!_collisionDetector) {
        _collisionDetector = new CollisionDetection();
    }
}

AnimSystem::~AnimSystem() {}

bool AnimSystem::step() {
    bool collision = false;
    Vec3 g = Vec3::Zero();
    if (gravity) {
        g.y() = -9.8f;
    }
    for (auto mesh: _meshes) {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i = 0; i<mesh->nodes.size(); i++) {
            auto node = mesh->nodes[i];
            node->force = g*node->mass;
        }
    }

    if (collisions) {
        collision = _collisionDetector->update();
    }

    _step();

    if (collisions) {
        _collisionDetector->checkLimitsCollision();
    }
    return collision;
}

void AnimSystem::addMesh(Mesh* mesh_) {
    _meshes.push_back( mesh_ );
    double density = mesh_->density;

    if (mesh_->tetrahedra.size()>0) {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i=0; i<mesh_->nodes.size(); i++) {
            auto node = mesh_->nodes[i];
            node->mass = 0.0;
        }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i=0; i<mesh_->tetrahedra.size(); i++) {
            auto tet = mesh_->tetrahedra[i];
            double massPerNode = tet->initVolume * density * 0.25;
            tet->node0->mass +=  massPerNode;
            tet->node1->mass +=  massPerNode;
            tet->node2->mass +=  massPerNode;
            tet->node3->mass +=  massPerNode;
        }
    } else {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i=0; i<mesh_->nodes.size(); i++) {
            auto node = mesh_->nodes[i];
            node->mass = density;
        }
    }
}

void AnimSystem::clear() {
    _meshes.clear();
}

}
