#include "Triangle.h"

namespace phyanim { 

Triangle::Triangle(Node* n0_, Node* n1_, Node* n2_)
    : node0(n0_), node1(n1_), node2(n2_) {
}

Triangle::~Triangle() {
}

Edges Triangle::edges() {
    Edges edges(3);
    edges[0] = new Edge(node0, node1);
    edges[1] = new Edge(node0, node2);
    edges[2] = new Edge(node1, node2);
    return edges;
}

double Triangle::area() {
    Vec3 axis0 = node1->position - node0->position;
    Vec3 axis1 = node2->position - node0->position;
    return axis0.cross(axis1).norm() * 0.5;
}

void Triangle::sortedIds(unsigned int& id0_, unsigned int& id1_,
                         unsigned int& id2_) const {
    id0_ = node0->id;
    id1_ = node1->id;
    id2_ = node2->id;
    if (id1_ < id0_) {
        std::swap(id0_, id1_);
    }
    if (id2_ < id1_) {
        std::swap(id1_, id2_);
        if (id1_ < id0_) {
            std::swap(id0_, id1_);
        }
    }
}


size_t TrianglePointerHash::operator()(const Triangle* triangle_) const {
    unsigned int id0, id1, id2;
    triangle_->sortedIds(id0, id1, id2);

    size_t result = id0;
    result = (result << 16) + id1;
    result = (result << 16) + id2;
    return result;
}

bool TrianglePointerEqual::operator()(const Triangle* triangle0_,
                                      const Triangle* triangle1_) const {
    unsigned int t0id0, t0id1, t0id2, t1id0, t1id1, t1id2;
    triangle0_->sortedIds(t0id0, t0id1, t0id2);
    triangle1_->sortedIds(t1id0, t1id1, t1id2);
    return (t0id0 == t1id0) && (t0id1 == t1id1) && (t0id2 == t1id2);
}

} 
