#include <Edge.h>

namespace phyanim {

Edge::Edge(Node* node0_, Node* node1_)
    : _n0(node0_), _n1(node1_) {
    _initLength = (_n1->position() - _n0->position()).norm();
}

Edge::~Edge() {}

Node* Edge::node0(void) {
    return _n0;
}

Node* Edge::node1(void) {
    return _n1;
}

Vec3 Edge::direction(void) {
    Vec3 direction = (_n1->position() - _n0->position());
    direction.normalize();
    return direction;
}

float Edge::length() {
    return (_n1->position() - _n0->position()).norm();
}

float Edge::lengthOffset() {
    return length() - _initLength;
}

}
