#include "Tetrahedron.h"

namespace phyanim{

Tetrahedron::Tetrahedron(Node* n0_, Node* n1_, Node* n2_, Node* n3_)
    : node0(n0_), node1(n1_), node2(n2_), node3(n3_) {
    Vec3 x0 = node0->position;
    Vec3 x1 = node1->position;
    Vec3 x2 = node2->position;
    Vec3 x3 = node3->position;

    normal0 = (x3-x1).cross(x2-x1).normalized();
    normal1 = (x2-x0).cross(x3-x0).normalized();
    normal2 = (x3-x0).cross(x1-x0).normalized();
    normal3 = (x1-x0).cross(x2-x0).normalized();

    basis << x1-x0, x2-x0, x3-x0;
    initVolume = std::abs(basis.determinant()/6.0);
    basis = basis.inverse().eval();
}

Tetrahedron::~Tetrahedron(void) {}


double Tetrahedron::volume() const {
    Vec3 x0 = node0->position;
    Vec3 x1 = node1->position;
    Vec3 x2 = node2->position;
    Vec3 x3 = node3->position;

    Mat3 basis;
    basis << x1-x0, x2-x0, x3-x0;
    return std::abs(basis.determinant()/6.0);
}

Edges Tetrahedron::edges(void) {
    Edges edges(6);
    edges[0] = new Edge(node0,node1);
    edges[1] = new Edge(node0,node2);
    edges[2] = new Edge(node0,node3);
    edges[3] = new Edge(node1,node2);
    edges[4] = new Edge(node1,node3);
    edges[5] = new Edge(node2,node3);

    return edges;
}


Triangles Tetrahedron::triangles(void) {
    Triangles triangles(4);

    triangles[0] = new Triangle(node0, node1, node2);
    triangles[1] = new Triangle(node0, node2, node3);
    triangles[2] = new Triangle(node0, node3, node1);
    triangles[3] = new Triangle(node1, node3, node2);

    return triangles;
}

}
