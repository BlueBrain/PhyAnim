#include "DrawableMesh.h"

#include <GL/glew.h>

#include <iostream>

#include "Tetrahedron.h"
#include "Triangle.h"

namespace phyanim
{
DrawableMesh::DrawableMesh(double stiffness_,
                           double density_,
                           double damping_,
                           double poissonRatio_)
    : Mesh(stiffness_, density_, damping_, poissonRatio_)
    , updatedPositions(false)
    , updatedColors(false)
    , _vao(0)
{
}

DrawableMesh::~DrawableMesh() {}

void DrawableMesh::load(const std::string& file_) { Mesh::load(file_); }

void DrawableMesh::load(const std::string& nodeFile_,
                        const std::string& eleFile_)
{
    Mesh::load(nodeFile_, eleFile_);
}

void DrawableMesh::render()
{
    upload();
    if (_vao != 0)
    {
        glBindVertexArray(_vao);
        glDrawElements(GL_TRIANGLES, _indicesSize, GL_UNSIGNED_INT, 0);
    }
}

void DrawableMesh::upload()
{
    if (_vao == 0)
    {
        glGenVertexArrays(1, &_vao);

        unsigned int vbos[3];
        glGenBuffers(3, vbos);
        _posVbo = vbos[0];
        _colorVbo = vbos[1];

        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, _colorVbo);
        glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        size_t trianglesSize = surfaceTriangles.size();
        _indicesSize = trianglesSize * 3;
        unsigned int surfaceIndices[_indicesSize];
        for (size_t i = 0; i < trianglesSize; ++i)
        {
            auto triangle = dynamic_cast<TrianglePtr>(surfaceTriangles[i]);
            surfaceIndices[i * 3] = triangle->node0->id;
            surfaceIndices[i * 3 + 1] = triangle->node1->id;
            surfaceIndices[i * 3 + 2] = triangle->node2->id;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(unsigned int) * _indicesSize, &surfaceIndices,
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
        _uploadPositions();
        _uploadColors();
    }
    else
    {
        if (updatedPositions)
        {
            _uploadPositions();
            updatedPositions = false;
        }
        if (updatedColors)
        {
            _uploadColors();
            updatedColors = false;
        }
    }
}

void DrawableMesh::updateColor(phyanim::Vec3 color)
{
    for (auto node : nodes) node->color = color;
    updatedColors = true;
}

void DrawableMesh::updateColors(phyanim::Vec3 staticColor,
                                phyanim::Vec3 dynamicColor,
                                phyanim::Vec3 collideColor)
{
    for (auto node : nodes)
    {
        Vec3 color = staticColor;
        if (node->collide)
            color = collideColor;
        else if (node->anim)
            color = dynamicColor;
        node->color = color;
    }
    updatedColors = true;
}

void DrawableMesh::_uploadPositions()
{
    size_t nodesSize = nodes.size();
    double posBuffer[nodesSize * 3];
    for (size_t i = 0; i < nodesSize; ++i)
    {
        auto pos = nodes[i]->position;
        posBuffer[i * 3] = pos.x();
        posBuffer[i * 3 + 1] = pos.y();
        posBuffer[i * 3 + 2] = pos.z();
    }
    glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nodesSize * 3, &posBuffer,
                 GL_STATIC_DRAW);
}

void DrawableMesh::_uploadColors()
{
    size_t nodesSize = nodes.size();
    double colorBuffer[nodesSize * 3];
    for (size_t i = 0; i < nodesSize; ++i)
    {
        auto color = nodes[i]->color;
        colorBuffer[i * 3] = color.x();
        colorBuffer[i * 3 + 1] = color.y();
        colorBuffer[i * 3 + 2] = color.z();
    }

    glBindBuffer(GL_ARRAY_BUFFER, _colorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nodesSize * 3, &colorBuffer,
                 GL_STATIC_DRAW);
}

}  // namespace phyanim
