#include "Tetrahedron.h"

namespace phyanim{

Tetrahedron::Tetrahedron(Node* n0_, Node* n1_, Node* n2_, Node* n3_)
    : _n0(n0_), _n1(n1_), _n2(n2_), _n3(n3_) {
}

Tetrahedron::~Tetrahedron(void) {}

Node* Tetrahedron::node0(void) {
    return _n0;
}

void Tetrahedron::node0(Node* node_) {
    _n0 = node_;
}

Node* Tetrahedron::node1(void) {
    return _n1;
}

void Tetrahedron::node1(Node* node_) {
    _n1 = node_;
}

Node* Tetrahedron::node2(void) {
    return _n2;
}

void Tetrahedron::node2(Node* node_) {
    _n2 = node_;
}

Node* Tetrahedron::node3(void) {
    return _n3;
}

void Tetrahedron::node3(Node* node_) {
    _n3 = node_;
}

Edges Tetrahedron::edges(void) {
    Edges edges(6);
    edges[0] = new Edge(_n0,_n1);
    edges[1] = new Edge(_n0,_n2);
    edges[2] = new Edge(_n0,_n3);
    edges[3] = new Edge(_n1,_n2);
    edges[4] = new Edge(_n1,_n3);
    edges[5] = new Edge(_n2,_n3);

    return edges;
}


Triangles Tetrahedron::triangles(void) {
    Triangles triangles(4);

    triangles[0] = new Triangle(_n0, _n1, _n2);
    triangles[1] = new Triangle(_n0, _n2, _n3);
    triangles[2] = new Triangle(_n0, _n3, _n1);
    triangles[3] = new Triangle(_n1, _n3, _n2);

    return triangles;
}

}
