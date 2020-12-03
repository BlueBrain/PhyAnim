#include <Mesh.h>
#include <BoundingSphere.h>

namespace phyanim {

Mesh::Mesh(double stiffness_, double density_,
           double damping_, double poissonRatio_)
    : initVolume(0.0), stiffness(stiffness_), density(density_)
    , damping(damping_), poissonRatio(poissonRatio_) {
    aabb = new AxisAlignedBoundingBox();
}

Mesh::~Mesh(void) {}

double Mesh::volume() {
    double volume = 0;

    if (!tetrahedra.empty()) {
        for (auto tet: tetrahedra) {
            volume += tet->volume();
        }
    }
    return volume;
}

void Mesh::trianglesToEdges() {
    for (auto tri: triangles) {
        auto triEdges = tri->edges();
        edges.insert(edges.end(), triEdges.begin(), triEdges.end());
    }
}

void Mesh::tetsToNodes() {
    for (auto tet: tetrahedra) {
        nodes.push_back(tet->node0);
        nodes.push_back(tet->node1);
        nodes.push_back(tet->node2);
        nodes.push_back(tet->node3);
    }
}

void Mesh::tetsToEdges() {
    UniqueEdges uniqueEdges;
    for (auto tet: tetrahedra) {
        for (auto edge: tet->edges()) {
            uniqueEdges.insert(edge);
        }
    }
    edges.insert(edges.end(), uniqueEdges.begin(), uniqueEdges.end());
}

void Mesh::tetsToTriangles() {
    triangles.clear();
    surfaceTriangles.clear();
    UniqueTriangles surfaceTs;
    std::pair<UniqueTriangles::iterator,bool> insertionResult;
    for (auto tet: tetrahedra) {

        for (auto t: tet->triangles()){
            insertionResult = surfaceTs.insert(t);
            if (!insertionResult.second) {
                surfaceTs.erase(insertionResult.first);
                triangles.push_back(t);
            }
        }
        // for (auto t: triangles) {
        //     if (t->node0()->surface() && t->node1()->surface() &&
        //         t->node2()->surface()){
        //         _surfaceTriangles.push_back(t);
        //     }
        // }
    }
    
    triangles.insert(triangles.end(), surfaceTs.begin(), surfaceTs.end());
    surfaceTriangles.insert(surfaceTriangles.end(), surfaceTs.begin(),
                            surfaceTs.end());
}

void Mesh::nodesToInitPos() {
    for (auto node: nodes) {
        node->position = node->initPosition;
        node->velocity = node->initVelocity;
        
    }        
}

}
