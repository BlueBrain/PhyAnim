#include <iostream>

#include <MassSpringSystem.h>

namespace phyanim {

MassSpringSystem::MassSpringSystem()
    : AnimSystem() {
}

MassSpringSystem::~MassSpringSystem() {}

void MassSpringSystem::step(float dt_) {
    Vec3 g = Vec3::Zero();
    if (_gravity) {
        g.y() = -9.8f;
    }
    
    for (auto mesh: _meshes) {

        auto ks = mesh->stiffness();
        auto kd = 1000.0 * mesh->damping();
        for (auto node: mesh->nodes()) {
            node->force(g*node->mass());
        }

        for (auto edge: mesh->edges()) {
            Vec3 d = edge->node1()->position() - edge->node0()->position();
            double r = edge->restLength();
            double l = d.norm();
            Vec3 v = edge->node1()->velocity() - edge->node0()->velocity();
            Vec3 f0 = Vec3::Zero();
            if (l > 0.0) {
                f0 = (ks*(l/r - 1.0) + kd*(v.dot(d) / (l*r))) * d/l;
            }
            edge->node0()->force(f0 + edge->node0()->force());
            edge->node1()->force(-f0 + edge->node1()->force());
        }
        

        for (auto node: mesh->nodes()) { 
            Vec3 a = node->force() / node->mass();
            Vec3 v = node->velocity() + a * dt_;
            Vec3 x = node->position() + v * dt_;
            if (_floor && x.y() <= _floorHeight) {
                v.y() = 0.0f;
                x.y() = _floorHeight;
            }
            node->velocity(v);
            node->position(x);
                
        }
    }
}

void MassSpringSystem::addMesh(Mesh* mesh_) {
    AnimSystem::addMesh(mesh_);
    double density = mesh_->density();
    for (auto node: mesh_->nodes()) {
        node->mass(0.0);
    }
    for (auto tet: mesh_->tetrahedra()) {
        tet->node0()->mass( tet->volume() * density * 0.25 +
                            tet->node0()->mass());
        tet->node1()->mass( tet->volume() * density * 0.25 +
                            tet->node1()->mass());
        tet->node2()->mass( tet->volume() * density * 0.25 +
                            tet->node2()->mass());
        tet->node3()->mass( tet->volume() * density * 0.25 +
                            tet->node3()->mass());
    }
}


}
