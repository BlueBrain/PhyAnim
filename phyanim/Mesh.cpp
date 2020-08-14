#include <Mesh.h>

namespace phyanim {

Mesh::Mesh(double stiffness_, double density_,
           double damping_, double poissonRatio_)
    : _stiffness(stiffness_), _density(density_)
    , _damping(damping_), _poissonRatio(poissonRatio_) {
    
}

Mesh::~Mesh(void) {}

Nodes& Mesh::nodes() {
    return _nodes;
}

Edges& Mesh::edges() {
    return _edges;
}

Tetrahedra& Mesh::tetrahedra() {
    return _tetrahedra;
}

Triangles& Mesh::surfaceTriangles() {
    return _surfaceTriangles;
}

Triangles& Mesh::triangles() {
    return _triangles;
}

double Mesh::stiffness() {
    return _stiffness;
}

void Mesh::stiffness(double stiffness_) {
    _stiffness = stiffness_;
}

double Mesh::density() {
    return _density;
}

void Mesh::density(double density_) {
    _density = density_;
}

double Mesh::damping() {
    return _damping;
}

void Mesh::damping(double damping_) {
    _damping = damping_; 
}

double Mesh::poissonRatio() {
    return _poissonRatio;
}

void Mesh::poissonRatio(double poissonRatio_) {
    _poissonRatio = poissonRatio_;
}

void Mesh::trianglesToEdges() {
    for (auto tri: _triangles) {
        auto edges = tri->edges();
        _edges.insert(_edges.end(), edges.begin(), edges.end());
    }
}

void Mesh::tetsToNodes() {
    for (auto tet: _tetrahedra) {
        _nodes.push_back(tet->node0());
        _nodes.push_back(tet->node1());
        _nodes.push_back(tet->node2());
        _nodes.push_back(tet->node3());
    }
}

void Mesh::tetsToEdges() {
    UniqueEdges uniqueEdges;
    for (auto tet: _tetrahedra) {
        for (auto edge: tet->edges()) {
            uniqueEdges.insert(edge);
        }
    }
    _edges.insert(_edges.end(), uniqueEdges.begin(), uniqueEdges.end());
            
}

void Mesh::tetsToTriangles() {
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

void Mesh::nodesToInitPos() {
    for (auto node: _nodes) {
        node->position(node->initPosition());
        node->velocity(Vec3::Zero());
    }        
}

}
