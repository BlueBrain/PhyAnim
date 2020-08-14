#include "Triangle.h"

namespace phyanim { 

Triangle::Triangle(Node* n0_, Node* n1_, Node* n2_)
    : _n0(n0_), _n1(n1_), _n2(n2_) {
}

Triangle::~Triangle() {
}

Node* Triangle::node0() {
    return _n0;
}

void Triangle::node0(Node* node_) {
    _n0 = node_;
}

Node* Triangle::node1() {
    return _n1;
}

void Triangle::node1(Node* node_) {
    _n1 = node_;
}

Node* Triangle::node2() {
    return _n2;
}

void Triangle::node2(Node* node_) {
    _n2 = node_;
}

Edges Triangle::edges() {
    Edges edges(3);
    edges[0] = new Edge(_n0, _n1);
    edges[1] = new Edge(_n0, _n2);
    edges[2] = new Edge(_n1, _n2);
    return edges;
}

} 
