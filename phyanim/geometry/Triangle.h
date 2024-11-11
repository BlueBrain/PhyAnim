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
