#include <iostream>

#include <AnimSystem.h>

namespace phyanim {

AnimSystem::AnimSystem(CollisionDetection* collDetector_)
    : gravity(true), limitsCollision(true), _collisionDetector(collDetector_) {
    if (!_collisionDetector) {
        _collisionDetector = new CollisionDetection();
    }
}

AnimSystem::~AnimSystem() {}

void AnimSystem::step(float dt_) {
    Vec3 g = Vec3::Zero();
    if (gravity) {
        g.y() = -9.8f;
    }
    for (auto mesh: _meshes) {
        for (auto node: mesh->nodes) {
            node->force = g*node->mass;
        }
    }
    if (_collisionDetector->update())
    {
        std::cout << "Collision detected" << std::endl;
    }
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

void AnimSystem::_checkLimitsCollision() {
    if (limitsCollision) {
        _collisionDetector->checkLimitsCollision();
    }
}

}
