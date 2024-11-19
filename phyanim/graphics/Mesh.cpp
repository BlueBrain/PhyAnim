/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Mesh.h"

#include <GL/glew.h>

namespace phyanim
{
namespace graphics
{
Mesh::Mesh()
    : _initialized(false)
    , _updateIndices(false)
    , _updatePositions(false)
    , _updateNormals(false)
    , _updateColors(false)
    , _indexSize(0)
    , _vao(0)
    , _indexVbo(0)
    , _positionVbo(0)
    , _normalVbo(0)
    , _colorVbo(0)
{
}

Mesh::~Mesh() {}

void Mesh::render(uint32_t primitipeType)
{
    _init();
    _upload();
    if (_vao != 0)
    {
        glBindVertexArray(_vao);
        glDrawElements(primitipeType, _indexSize, GL_UNSIGNED_INT, 0);
    }
}

void Mesh::uploadIndices(std::vector<uint32_t> indices)
{
    _indices = indices;
    _updateIndices = true;
}
void Mesh::uploadPositions(std::vector<float> positions)
{
    _positions = positions;
    _updatePositions = true;
}
void Mesh::uploadNormals(std::vector<float> normals)
{
    _normals = normals;
    _updateNormals = true;
}
void Mesh::uploadColors(std::vector<float> colors)
{
    _colors = colors;
    _updateColors = true;
}

void Mesh::_init()
{
    if (!_initialized)
    {
        glGenVertexArrays(1, &_vao);
        unsigned int vbos[4];
        glGenBuffers(4, vbos);
        _indexVbo = vbos[0];
        _positionVbo = vbos[1];
        _normalVbo = vbos[2];
        _colorVbo = vbos[3];
        glBindVertexArray(_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _positionVbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _normalVbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, _colorVbo);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        _initialized = true;
    }
}

void Mesh::_upload()
{
    if (_updateIndices) _uploadIndices();
    if (_updatePositions) _uploadPositions();
    if (_updateNormals) _uploadNormals();
    if (_updateColors) _uploadColors();
}

void Mesh::_uploadIndices()
{
    _indexSize = _indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indexSize,
                 _indices.data(), GL_STATIC_DRAW);
    _indices.clear();
    _updateIndices = false;
}

void Mesh::_uploadPositions()
{
    glBindBuffer(GL_ARRAY_BUFFER, _positionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _positions.size(),
                 _positions.data(), GL_STATIC_DRAW);
    _positions.clear();
    _updatePositions = false;
}

void Mesh::_uploadNormals()
{
    glBindBuffer(GL_ARRAY_BUFFER, _normalVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _normals.size(),
                 _normals.data(), GL_STATIC_DRAW);
    _normals.clear();
    _updateNormals = false;
}

void Mesh::_uploadColors()
{
    glBindBuffer(GL_ARRAY_BUFFER, _colorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _colors.size(),
                 _colors.data(), GL_STATIC_DRAW);
    _colors.clear();
    _updateColors = false;
}

void setGeometry(Mesh* mesh, geometry::Nodes& nodes)
{
    uint32_t numNodes = nodes.size();
    std::vector<float> positions(numNodes * 3);
    std::vector<float> normals(numNodes * 3);
    for (uint32_t i = 0; i < numNodes; ++i)
    {
        vec3ToVec(nodes[i]->position, positions, i);
        vec3ToVec(nodes[i]->normal, normals, i);
    }
    mesh->uploadPositions(positions);
    mesh->uploadNormals(normals);
};

void setColor(Mesh* mesh, uint32_t numNodes, geometry::Vec3 color)
{
    std::vector<float> colors(numNodes * 3);
    for (uint32_t i = 0; i < numNodes; ++i)
    {
        vec3ToVec(color, colors, i);
    }
    mesh->uploadColors(colors);
};

void setColorByCollision(Mesh* mesh,
                         geometry::Nodes& nodes,
                         geometry::Vec3 noCollisionColor,
                         geometry::Vec3 collisionColor)
{
    uint32_t numNodes = nodes.size();
    std::vector<float> colors(numNodes * 3);
    for (uint32_t i = 0; i < numNodes; ++i)
    {
        auto color = noCollisionColor;
        if (nodes[i]->collide) color = collisionColor;
        vec3ToVec(color, colors, i);
    }
    mesh->uploadColors(colors);
}

void setIndices(Mesh* mesh, geometry::Primitives& primitives)
{
    uint32_t numTriangles = primitives.size();
    std::vector<uint32_t> indices(numTriangles * 3);
    for (uint32_t i = 0; i < numTriangles; ++i)
    {
        auto nodes = primitives[i]->nodes();
        indices[i * 3] = nodes[0]->id;
        indices[i * 3 + 1] = nodes[1]->id;
        indices[i * 3 + 2] = nodes[2]->id;
    }
    mesh->uploadIndices(indices);
};

Mesh* generateMesh(geometry::Nodes& nodes,
                   geometry::Primitives& primitives,
                   geometry::Vec3 color)
{
    Mesh* mesh = new Mesh();
    setGeometry(mesh, nodes);
    setColor(mesh, nodes.size(), color);
    setIndices(mesh, primitives);
    return mesh;
};

Mesh* generateMesh(geometry::Edges& edges,
                   geometry::Vec3 color,
                   geometry::Vec3 collColor,
                   geometry::Vec3 fixColor)
{
    std::vector<uint32_t> indices;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<uint32_t> sectionIndices;
    std::vector<float> sectionPositions;
    std::vector<float> sectionNormals;
    std::vector<float> sectionColors;
    for (auto edge : edges)
    {
        computeEdgeIndices(sectionIndices, positions.size() / 3);
        computeEdgeGeometry(dynamic_cast<geometry::Edge*>(edge),
                            sectionPositions, sectionNormals, sectionColors,
                            color, collColor, fixColor);

        indices.insert(indices.end(), sectionIndices.begin(),
                       sectionIndices.end());
        positions.insert(positions.end(), sectionPositions.begin(),
                         sectionPositions.end());
        normals.insert(normals.end(), sectionNormals.begin(),
                       sectionNormals.end());
        colors.insert(colors.end(), sectionColors.begin(), sectionColors.end());
    }

    Mesh* mesh = new Mesh();
    mesh->uploadIndices(indices);
    mesh->uploadPositions(positions);
    mesh->uploadNormals(normals);
    mesh->uploadColors(colors);
    return mesh;
}

void computeEdgeGeometry(geometry::Edge* edge,
                         std::vector<float>& positions,
                         std::vector<float>& normals,
                         std::vector<float>& colors,
                         geometry::Vec3 color,
                         geometry::Vec3 collColor,
                         geometry::Vec3 fixColor)
{
    positions.resize(10 * 3);
    normals.resize(10 * 3);
    colors.resize(10 * 3);

    auto p0 = edge->node0->position;
    auto p1 = edge->node1->position;
    auto r0 = edge->node0->radius;
    auto r1 = edge->node1->radius;

    geometry::Vec3 t;
    if (glm::distance(p1, p0) < 0.001f)
        t = geometry::Vec3(0, 1, 0);
    else
        t = glm::normalize(p1 - p0);

    geometry::Vec3 axis_y(0, 1, 0);
    if (t.y == 1.0f || t.y == -1.0f) axis_y = geometry::Vec3(-1, 0, 0);

    geometry::Vec3 axis_x = glm::normalize(glm::cross(axis_y, t));
    axis_y = glm::normalize(glm::cross(t, axis_x));

    vec3ToVec(p0 - axis_x * r0, positions, 0);
    vec3ToVec(p0 - axis_y * r0, positions, 1);
    vec3ToVec(p0 + axis_x * r0, positions, 2);
    vec3ToVec(p0 + axis_y * r0, positions, 3);
    vec3ToVec(p0 - t * r0, positions, 4);

    vec3ToVec(p1 - axis_x * r1, positions, 5);
    vec3ToVec(p1 - axis_y * r1, positions, 6);
    vec3ToVec(p1 + axis_x * r1, positions, 7);
    vec3ToVec(p1 + axis_y * r1, positions, 8);
    vec3ToVec(p1 + t * r1, positions, 9);

    vec3ToVec(-axis_x * r0, normals, 0);
    vec3ToVec(-axis_y * r0, normals, 1);
    vec3ToVec(axis_x * r0, normals, 2);
    vec3ToVec(axis_y * r0, normals, 3);
    vec3ToVec(-t * r0, normals, 4);

    vec3ToVec(-axis_x * r1, normals, 5);
    vec3ToVec(-axis_y * r1, normals, 6);
    vec3ToVec(axis_x * r1, normals, 7);
    vec3ToVec(axis_y * r1, normals, 8);
    vec3ToVec(t * r1, normals, 9);

    auto c = color;
    if (edge->node0->collide) c = collColor;
    if (edge->node0->fix || edge->node1->isSoma) c = fixColor;
    for (uint32_t i = 0; i < 5; ++i) vec3ToVec(c, colors, i);

    c = color;
    if (edge->node1->collide) c = collColor;
    if (edge->node1->fix || edge->node1->isSoma) c = fixColor;
    for (uint32_t i = 5; i < 10; ++i) vec3ToVec(c, colors, i);
}

void computeEdgeIndices(std::vector<uint32_t>& indices, uint32_t offset)
{
    indices.resize(48);

    uint32_t id0 = offset;
    uint32_t id1 = offset + 5;

    indices[0] = id0;
    indices[1] = id0 + 1;
    indices[2] = id1;
    indices[3] = id0 + 1;
    indices[4] = id1 + 1;
    indices[5] = id1;

    indices[6] = id0 + 1;
    indices[7] = id0 + 2;
    indices[8] = id1 + 1;
    indices[9] = id0 + 2;
    indices[10] = id1 + 2;
    indices[11] = id1 + 1;

    indices[12] = id0 + 2;
    indices[13] = id0 + 3;
    indices[14] = id1 + 2;
    indices[15] = id0 + 3;
    indices[16] = id1 + 3;
    indices[17] = id1 + 2;

    indices[18] = id0 + 3;
    indices[19] = id0;
    indices[20] = id1 + 3;
    indices[21] = id0;
    indices[22] = id1;
    indices[23] = id1 + 3;

    indices[24] = id0 + 1;
    indices[25] = id0;
    indices[26] = id0 + 4;
    indices[27] = id0 + 2;
    indices[28] = id0 + 1;
    indices[29] = id0 + 4;
    indices[30] = id0 + 3;
    indices[31] = id0 + 2;
    indices[32] = id0 + 4;
    indices[33] = id0;
    indices[34] = id0 + 3;
    indices[35] = id0 + 4;

    indices[36] = id1;
    indices[37] = id1 + 1;
    indices[38] = id1 + 4;
    indices[39] = id1 + 1;
    indices[40] = id1 + 2;
    indices[41] = id1 + 4;
    indices[42] = id1 + 2;
    indices[43] = id1 + 3;
    indices[44] = id1 + 4;
    indices[45] = id1 + 3;
    indices[46] = id1;
    indices[47] = id1 + 4;
}

void updateGeometry(Mesh* mesh,
                    geometry::Edges& edges,
                    geometry::Vec3 color,
                    geometry::Vec3 collColor,
                    geometry::Vec3 fixColor)
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<float> sectionPositions;
    std::vector<float> sectionNormals;
    std::vector<float> sectionColors;

    for (auto edge : edges)
    {
        computeEdgeGeometry(dynamic_cast<geometry::Edge*>(edge),
                            sectionPositions, sectionNormals, sectionColors,
                            color, collColor, fixColor);
        positions.insert(positions.end(), sectionPositions.begin(),
                         sectionPositions.end());
        normals.insert(normals.end(), sectionNormals.begin(),
                       sectionNormals.end());
        colors.insert(colors.end(), sectionColors.begin(), sectionColors.end());
    }

    mesh->uploadPositions(positions);
    mesh->uploadNormals(normals);
    mesh->uploadColors(colors);
}

void vec3ToVec(geometry::Vec3 vec3, std::vector<float>& vec, uint32_t index)
{
    vec[index * 3] = vec3.x;
    vec[index * 3 + 1] = vec3.y;
    vec[index * 3 + 2] = vec3.z;
}

}  // namespace graphics
}  // namespace phyanim
