/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __EXAMPLES_MESH_H
#define __EXAMPLES_MESH_H

#include "../geometry/Edge.h"

namespace phyanim
{
namespace graphics
{
class Mesh
{
public:
    Mesh();

    virtual ~Mesh();

    void render(uint32_t primitiveType = 0x0004);

    void uploadIndices(std::vector<uint32_t> indices);
    void uploadPositions(std::vector<float> positions);
    void uploadNormals(std::vector<float> normals);
    void uploadColors(std::vector<float> colors);

private:
    std::vector<uint32_t> _indices;
    std::vector<float> _positions;
    std::vector<float> _normals;
    std::vector<float> _colors;

    bool _initialized;
    bool _updateIndices;
    bool _updatePositions;
    bool _updateNormals;
    bool _updateColors;

    void _init();
    void _upload();
    void _uploadIndices();
    void _uploadPositions();
    void _uploadNormals();
    void _uploadColors();

    uint32_t _vao;
    uint32_t _indexVbo;
    uint32_t _positionVbo;
    uint32_t _normalVbo;
    uint32_t _colorVbo;
    uint32_t _indexSize;
};

void setGeometry(Mesh* mesh, geometry::Nodes& nodes);

void setColor(Mesh* mesh,
              uint32_t numNodes,
              geometry::Vec3 color = geometry::Vec3(0, 0.8, 0.8));

void setColorByCollision(
    Mesh* mesh,
    geometry::Nodes& nodes,
    geometry::Vec3 noCollisionColor = geometry::Vec3(0, 0.8, 0.8),
    geometry::Vec3 collisionColor = geometry::Vec3(0.8, 0.2, 0));

void setIndices(Mesh* mesh, geometry::Primitives& primitives);

Mesh* generateMesh(geometry::Nodes& nodes,
                   geometry::Primitives& primitives,
                   geometry::Vec3 color = geometry::Vec3(0, 0.8, 0.8));

void vec3ToVec(geometry::Vec3 vec3, std::vector<float>& vec, uint32_t index);

void computeEdgeGeometry(geometry::Edge* edge,
                         std::vector<float>& positions,
                         std::vector<float>& normals,
                         std::vector<float>& colors,
                         geometry::Vec3 color,
                         geometry::Vec3 collColor,
                         geometry::Vec3 fixColor);

void computeEdgeIndices(std::vector<uint32_t>& indices, uint32_t offset);

void updateGeometry(Mesh* mesh,
                    geometry::Edges& edges,
                    geometry::Vec3 color = geometry::Vec3(0.2, 0.8, 0.2),
                    geometry::Vec3 collColor = geometry::Vec3(0.8, 0.2, 0.2),
                    geometry::Vec3 fixColor = geometry::Vec3(0.2, 0.2, 0.2));

Mesh* generateMesh(geometry::Edges& edges,
                   geometry::Vec3 color = geometry::Vec3(0.2, 0.8, 0.2),
                   geometry::Vec3 collColor = geometry::Vec3(0.8, 0.2, 0.2),
                   geometry::Vec3 fixColor = geometry::Vec3(0.2, 0.2, 0.2));

}  // namespace graphics
}  // namespace phyanim

#endif
