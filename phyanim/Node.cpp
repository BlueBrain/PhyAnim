#include "Node.h"

namespace phyanim{

Node::Node(Vec3 position_, unsigned int id_, bool surface_,
            Vec3 v0_, Vec3 a0_)
    : _position(position_), _initPosition(position_), _id(id_)
    , _surface(surface_), _v0(v0_), _a0(a0_) {

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

void Node::initPosition(Vec3 initPosition_) {
    _initPosition = initPosition_;
}


Vec3 Node::v0() const {
    return _v0;
}

void Node::v0(Vec3 v0_) {
    _v0 = v0_;
}

Vec3 Node::a0() const {
    return _a0;
}

void Node::a0(Vec3 a0_) {
    _a0 = a0_;
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
