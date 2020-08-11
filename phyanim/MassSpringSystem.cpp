#include <iostream>

#include <MassSpringSystem.h>

namespace phyanim {

MassSpringSystem::MassSpringSystem()
    : AnimSystem() {
}

MassSpringSystem::~MassSpringSystem() {}

void MassSpringSystem::step(float dt_) {
    Vec3 zeroVec(0.0f, 0.0f, 0.0f);
    Vec3 g(zeroVec);
    if (_gravity) {
        g = Vec3(0.0f, -9.8f, 0.0f);
    }
    
    for (auto mesh: _meshes) {

        auto stiffness = mesh->stiffness();
        for (auto node: mesh->nodes()) {
            node->a0(zeroVec);
        }

        for (auto edge: mesh->edges()) {
            auto f0 = edge->direction() * edge->lengthOffset() * stiffness;
            edge->node0()->a0(f0 + edge->node0()->a0());
            edge->node1()->a0(-f0 + edge->node1()->a0());
        }
        

        for (auto node: mesh->nodes()) {
            Vec3 a = node->a0() + g;
            Vec3 v = node->v0()+a*dt_;
            Vec3 x = node->position() + v*dt_;
            if (_floor && x.y() <= 0.0f) {
                v.y() = 0.0f;
                x.y() = 0.0f;
            }
            node->a0(a);
            node->v0(v);
            node->position(x);
            
        }
    }
}


}
