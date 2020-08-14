#include "Node.h"

namespace phyanim{

Node::Node(Vec3 position_, unsigned int id_, Vec3 velocity_, Vec3 force_,
           double mass_, bool surface_)                                                          
    : _initPosition(position_), _position(position_), _id(id_)
    , _velocity(velocity_), _force(force_), _mass(mass_)
    , _surface(surface_) {

}

Node::~Node() {

}

Vec3 Node::position() const {
    return _position;
}

void Node::position(Vec3 position_) {
    _position = position_;
}

Vec3 Node::initPosition() const {
    return _initPosition;
}


Vec3 Node::velocity() const {
    return _velocity;
}

void Node::velocity(Vec3 velocity_) {
    _velocity = velocity_;
}

Vec3 Node::force() const {
    return _force;
}

void Node::force(Vec3 force_) {
    _force = force_;
}

double Node::mass() const {
    return _mass;
}

void Node::mass(double mass_) {
    _mass = mass_;
}

unsigned int Node::id() const{
    return _id;
}

void Node::id( unsigned int id_ ){
    _id = id_;
}

bool Node::surface(){
    return _surface;
}

void Node::surface(bool surface_){
    _surface = surface_;
}

}
