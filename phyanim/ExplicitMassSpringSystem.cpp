#include <iostream>

#include <ExplicitMassSpringSystem.h>

namespace phyanim {

ExplicitMassSpringSystem::ExplicitMassSpringSystem(
    CollisionDetection* collDetector_)
    : AnimSystem(collDetector_) {
}

ExplicitMassSpringSystem::~ExplicitMassSpringSystem() {
    
}

void ExplicitMassSpringSystem::_step(double dt_) {
    for (auto mesh: _meshes) {
        auto ks = mesh->stiffness;
        auto kd = 1000.0 * mesh->damping;
  
        for (auto edge: mesh->edges) {
            Vec3 d = edge->node1->position - edge->node0->position;
            double r = edge->resLength;
            double l = d.norm();
            Vec3 v = edge->node1->velocity - edge->node0->velocity;
            Vec3 f0 = Vec3::Zero();
            if (l > 0.0) {
                f0 = (ks*(l/r - 1.0) + kd*(v.dot(d) / (l*r))) * d/l;
            }
            edge->node0->force += f0;
            edge->node1->force += -f0;
        }
        
        for (auto node: mesh->nodes) { 
            Vec3 a = node->force / node->mass;
            Vec3 v = node->velocity + a * dt_;
            Vec3 x = node->position + v * dt_;
            node->velocity = v;
            node->position = x;
        }
    }
}

}
