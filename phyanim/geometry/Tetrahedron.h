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

#ifndef __PHYANIM_TETRAHEDRON__
#define __PHYANIM_TETRAHEDRON__

#include "Edge.h"
#include "Triangle.h"

namespace phyanim
{
namespace geometry
{
class Tetrahedron;

typedef Tetrahedron* TetrahedronPtr;

typedef std::vector<TetrahedronPtr> Tetrahedra;

class Tetrahedron : public Primitive
{
public:
    Tetrahedron(Node* n0_, Node* n1_, Node* n2_, Node* n3_);

    ~Tetrahedron() {}

    Node* node0;

    Node* node1;

    Node* node2;

    Node* node3;

    float initVolume();

    float volume() const;

    Nodes nodes() const;

    Edges edges() const;

    Triangles triangles();

private:
    float _volume;
    bool _volumeComputed;
};

}  // namespace geometry
}  // namespace phyanim

#endif
