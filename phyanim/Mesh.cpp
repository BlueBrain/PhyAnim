#include <Mesh.h>

namespace phyanim {

Mesh::Mesh(float stiffness_)
    : _stiffness(stiffness_){
    
}

Mesh::~Mesh(void) {}

Nodes& Mesh::nodes(void) {
    return _nodes;
}

Edges& Mesh::edges(void) {
    return _edges;
}

Tetrahedra& Mesh::tetrahedra(void) {
    return _tetrahedra;
}

Triangles& Mesh::surfaceTriangles(void) {
    return _surfaceTriangles;
}

Triangles& Mesh::triangles(void) {
    return _triangles;
}

float Mesh::stiffness(void) {
    return _stiffness;
}

void Mesh::stiffness(float stiffness_) {
    _stiffness = stiffness_;
}

void Mesh::tetsToNodes(void) {
    for (auto tet: _tetrahedra) {
        _nodes.push_back(tet->node0());
        _nodes.push_back(tet->node1());
        _nodes.push_back(tet->node2());
        _nodes.push_back(tet->node3());
    }
}

void Mesh::tetsToEdges(void) {
    for (auto tet: _tetrahedra) {
        auto edges = tet->edges();
        _edges.insert(_edges.end(), edges.begin(), edges.end());
    }
}

void Mesh::tetsToTriangles(void) {
    for (auto tet: _tetrahedra) {
        //TODO delete duplicated triangles
        auto triangles = tet->triangles();
        for (auto t: triangles) {
            if (t->node0()->surface() && t->node1()->surface() &&
                t->node2()->surface()){
                _surfaceTriangles.push_back(t);
            }
        }
        _triangles.insert(_triangles.end(), triangles.begin(), triangles.end());
        
    }
}

void Mesh::nodesToInitPos(void) {
    for (auto node: _nodes) {
        node->position(node->initPosition());
    }        
}

}
