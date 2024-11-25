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

#ifndef __PHYANIM_TRIANGLE__
#define __PHYANIM_TRIANGLE__

#include "Edge.h"
#include "Primitive.h"

namespace phyanim
{
namespace geometry
{
class Triangle;

typedef Triangle* TrianglePtr;

typedef std::vector<TrianglePtr> Triangles;

struct TrianglePointerHash
{
public:
    size_t operator()(const Triangle* triangle_) const;
};

struct TrianglePointerEqual
{
public:
    bool operator()(const Triangle* triangle0_,
                    const Triangle* triangle_) const;
};

typedef std::
    unordered_set<TrianglePtr, TrianglePointerHash, TrianglePointerEqual>
        UniqueTriangles;

class Triangle : public Primitive
{
public:
    Triangle(Node* n0_, Node* n1_, Node* n2_);

    virtual ~Triangle();

    Nodes nodes() const;

    Edges edges() const;

    float area() const;

    Vec3 normal() const;

    void sortedIds(unsigned int& id0_,
                   unsigned int& id1_,
                   unsigned int& id2_) const;

public:
    Node* node0;

    Node* node1;

    Node* node2;
};

typedef std::pair<TrianglePtr, TrianglePtr> TrianglePair;

typedef std::vector<TrianglePair> TrianglePairs;

}  // namespace geometry
}  // namespace phyanim

#endif
