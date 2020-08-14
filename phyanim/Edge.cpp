#include <Edge.h>

namespace phyanim {

Edge::Edge(Node* node0_, Node* node1_)
    : _n0(node0_), _n1(node1_) {
    _initLength = (_n1->position() - _n0->position()).norm();
}

Edge::~Edge() {}

Node* Edge::node0() {
    return _n0;
}

Node* Edge::node0() const{
    return _n0;
}

Node* Edge::node1() {
    return _n1;
}

Node* Edge::node1() const{
    return _n1;
}

double Edge::restLength() {
    return _initLength;
}

bool Edge::operator==(const Edge& edge_) const {
    unsigned int id0 = _n0->id();
    unsigned int id1 = _n1->id();
    unsigned int oId0 = edge_.node0()->id();
    unsigned int oId1 = edge_.node1()->id();
    if (id1 > id0) {
        std::swap(id1, id0);
    }
    if (oId1 > oId0) {
        std::swap(oId1, oId0);
    }
    return (id0 == oId0) & (id1 == oId1);
}

size_t EdgePointerHash::operator()(const Edge* edge_) const {
    unsigned int id0 = edge_->node0()->id();
    unsigned int id1 = edge_->node1()->id();
    if (id1 > id0) {
        std::swap(id1, id0);
    }
    return std::hash<unsigned int>{}(id0)^std::hash<unsigned int>{}(id1);
}

bool EdgePointerEqual::operator()(const Edge* edge0_, const Edge* edge1_) const {
    unsigned int e0id0 = edge0_->node0()->id();
    unsigned int e0id1 = edge0_->node1()->id();
    unsigned int e1id0 = edge1_->node0()->id();
    unsigned int e1id1 = edge1_->node1()->id();
    if (e0id1 > e0id0) {
        std::swap(e0id0, e0id1);
    }
    if (e1id1 > e1id0) {
        std::swap(e1id0, e1id1);
    }
    return (e0id0 == e1id0) && (e0id1 == e1id1);
}

}
