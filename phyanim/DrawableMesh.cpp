#include <iostream>

#include <GL/glew.h>

#include <DrawableMesh.h>

namespace phyanim {

DrawableMesh::DrawableMesh(double stiffness_, double density_,
                           double damping_, double poissonRatio_)
    : Mesh( stiffness_, density_, damping_, poissonRatio_) {
    _vao.resize(2);
}

DrawableMesh::~DrawableMesh() {}

void DrawableMesh::render() {
    glBindVertexArray(_vao[0]);
    glDrawElements(GL_TRIANGLES, _indicesSize, GL_UNSIGNED_INT, 0);
}

void DrawableMesh::renderSurface() {
    glBindVertexArray(_vao[1]);
    glDrawElements(GL_TRIANGLES, _surfaceIndicesSize, GL_UNSIGNED_INT, 0);
}

void DrawableMesh::upload() {
    glGenVertexArrays(2, _vao.data());
    unsigned int vbos[3];
    glGenBuffers(3, vbos);
    _posVbo = vbos[0];
    
    size_t nodesSize = nodes.size();
    double posBuffer[nodesSize*3];
    for (size_t i = 0; i < nodesSize; ++i) {
        auto pos = nodes[i]->position;
        posBuffer[i*3] = pos.x();
        posBuffer[i*3+1] = pos.y();
        posBuffer[i*3+2] = pos.z();
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double)*nodesSize*3,
                 &posBuffer, GL_STATIC_DRAW);
 
    // glBindVertexArray(_vao[0]);

    // glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
    // glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(0);

    // size_t trianglesSize = triangles.size();
    // _indicesSize = trianglesSize*3;
    // unsigned int indices[_indicesSize];
    // for (size_t i = 0; i < trianglesSize; ++i) {
    //     auto triangle = triangles[i];
    //     indices[i*3] = triangle->node0->id;
    //     indices[i*3+1] = triangle->node1->id;
    //     indices[i*3+2] = triangle->node2->id;
    // }
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*_indicesSize,
    //              &indices, GL_STATIC_DRAW);

    glBindVertexArray(_vao[1]);

    glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    size_t trianglesSize = surfaceTriangles.size();
    _surfaceIndicesSize = trianglesSize*3;
    unsigned int surfaceIndices[_surfaceIndicesSize];
    for (size_t i = 0; i < trianglesSize; ++i) {
        auto triangle = surfaceTriangles[i];
        surfaceIndices[i*3] = triangle->node0->id;
        surfaceIndices[i*3+1] = triangle->node1->id;
        surfaceIndices[i*3+2] = triangle->node2->id;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int)*_surfaceIndicesSize,
                 &surfaceIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void DrawableMesh::uploadNodes() {
    
    size_t nodesSize = nodes.size();
    double posBuffer[nodesSize*3];
    for (size_t i = 0; i < nodesSize; ++i) {
        auto pos = nodes[i]->position;
        posBuffer[i*3] = pos.x();
        posBuffer[i*3+1] = pos.y();
        posBuffer[i*3+2] = pos.z();
    } 
    glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double)*nodesSize*3,
                 &posBuffer, GL_STATIC_DRAW);
}

}
