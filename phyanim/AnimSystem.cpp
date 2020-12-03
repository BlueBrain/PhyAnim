#include <iostream>

#include <AnimSystem.h>

namespace phyanim {

AnimSystem::AnimSystem(CollisionDetection* collDetector_)
    : gravity(true), collisions(true), _collisionDetector(collDetector_) {
    if (!_collisionDetector) {
        _collisionDetector = new CollisionDetection();
    }
}

AnimSystem::~AnimSystem() {}

bool AnimSystem::step(double dt_) {
    bool collision = false;
    Vec3 g = Vec3::Zero();
    if (gravity) {
        g.y() = -9.8f;
    }
    for (auto mesh: _meshes) {
        for (auto node: mesh->nodes) {
            node->force = g*node->mass;
        }
    }

    if (collisions) {
        collision = _collisionDetector->update();
    }

    _step(dt_);

    if (collisions) {
        _collisionDetector->checkLimitsCollision();
    }
    return collision;
}

void AnimSystem::addMesh(Mesh* mesh_) {
    _meshes.push_back( mesh_ );
    _collisionDetector->addMesh(mesh_);
    double density = mesh_->density;
    for (auto node: mesh_->nodes) {
        node->mass = 0.0;
    }
    for (auto tet: mesh_->tetrahedra) {
        double massPerNode = tet->initVolume * density * 0.25;
        tet->node0->mass +=  massPerNode;
        tet->node1->mass +=  massPerNode;
        tet->node2->mass +=  massPerNode;
        tet->node3->mass +=  massPerNode;
    }
}

void AnimSystem::clear() {
    _collisionDetector->clear();    
    _meshes.clear();
}

}
