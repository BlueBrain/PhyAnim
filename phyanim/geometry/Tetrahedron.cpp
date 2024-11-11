/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
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
