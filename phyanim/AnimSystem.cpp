#include <AnimSystem.h>

namespace phyanim {

AnimSystem::AnimSystem()
    : _gravity(false), _floor(true) {
    
}

AnimSystem::~AnimSystem() {}

void AnimSystem::addMesh(phyanim::Mesh *mesh_) {
    _meshes.push_back( mesh_ );
}

void AnimSystem::gravity(bool gravity_) {
    _gravity = gravity_;
}

bool AnimSystem::gravity() {
    return _gravity;
}

void AnimSystem::floorCollision(bool floorCollision_) {
    _floor = floorCollision_;
}

bool AnimSystem::floorCollision() {
    return _floor;
}

}
