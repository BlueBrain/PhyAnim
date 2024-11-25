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

#include "Tetrahedron.h"

#include <iostream>

namespace phyanim
{
namespace geometry
{
Tetrahedron::Tetrahedron(Node* n0_, Node* n1_, Node* n2_, Node* n3_)
    : node0(n0_)
    , node1(n1_)
    , node2(n2_)
    , node3(n3_)
    , _volume(0.0)
    , _volumeComputed(false)
{
    update();
}

float Tetrahedron::initVolume()
{
    if (_volumeComputed) return _volume;

    Vec3 x0 = node0->initPosition;
    Vec3 x1 = node1->initPosition;
    Vec3 x2 = node2->initPosition;
    Vec3 x3 = node3->initPosition;

    Mat3 basis(x1 - x0, x2 - x0, x3 - x0);

    _volume = std::abs(glm::determinant(basis) / 6.0f);
    _volumeComputed = true;
    return _volume;
}

float Tetrahedron::volume() const
{
    Vec3 x0 = node0->position;
    Vec3 x1 = node1->position;
    Vec3 x2 = node2->position;
    Vec3 x3 = node3->position;

    Mat3 basis(x1 - x0, x2 - x0, x3 - x0);
    return std::abs(glm::determinant(basis) / 6.0f);
}

Nodes Tetrahedron::nodes() const
{
    Nodes nodes(4);
    nodes[0] = node0;
    nodes[1] = node1;
    nodes[2] = node2;
    nodes[3] = node3;
    return nodes;
}
Edges Tetrahedron::edges() const
{
    Edges edges(6);
    edges[0] = new Edge(node0, node1);
    edges[1] = new Edge(node0, node2);
    edges[2] = new Edge(node0, node3);
    edges[3] = new Edge(node1, node2);
    edges[4] = new Edge(node1, node3);
    edges[5] = new Edge(node2, node3);

    return edges;
}

Triangles Tetrahedron::triangles(void)
{
    Triangles triangles(4);

    triangles[0] = new Triangle(node0, node1, node3);
    triangles[1] = new Triangle(node0, node2, node1);
    triangles[2] = new Triangle(node0, node3, node2);
    triangles[3] = new Triangle(node1, node2, node3);

    return triangles;
}

}  // namespace geometry
}  // namespace phyanim
