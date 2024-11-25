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
