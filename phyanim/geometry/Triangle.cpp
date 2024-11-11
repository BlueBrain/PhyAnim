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

#include "Triangle.h"

namespace phyanim
{
namespace geometry
{
Triangle::Triangle(Node* n0_, Node* n1_, Node* n2_)
    : node0(n0_)
    , node1(n1_)
    , node2(n2_)
{
    update();
}

Triangle::~Triangle() {}

Nodes Triangle::nodes() const
{
    Nodes nodes(3);
    nodes[0] = node0;
    nodes[1] = node1;
    nodes[2] = node2;
    return nodes;
}

Edges Triangle::edges() const
{
    Edges edges(3);
    edges[0] = new Edge(node0, node1);
    edges[1] = new Edge(node0, node2);
    edges[2] = new Edge(node1, node2);
    return edges;
}

float Triangle::area() const
{
    Vec3 axis0 = node1->position - node0->position;
    Vec3 axis1 = node2->position - node0->position;
    return glm::cross(axis0, axis1).length() * 0.5;
}

Vec3 Triangle::normal() const
{
    Vec3 axis0 = node1->position - node0->position;
    Vec3 axis1 = node2->position - node0->position;
    return glm::normalize(glm::cross(axis0, axis1));
}

void Triangle::sortedIds(unsigned int& id0_,
                         unsigned int& id1_,
                         unsigned int& id2_) const
{
    id0_ = node0->id;
    id1_ = node1->id;
    id2_ = node2->id;
    if (id1_ < id0_)
    {
        std::swap(id0_, id1_);
    }
    if (id2_ < id1_)
    {
        std::swap(id1_, id2_);
        if (id1_ < id0_)
        {
            std::swap(id0_, id1_);
        }
    }
}

size_t TrianglePointerHash::operator()(const Triangle* triangle_) const
{
    unsigned int id0, id1, id2;
    triangle_->sortedIds(id0, id1, id2);

    size_t result = id0;
    result = (result << 16) + id1;
    result = (result << 16) + id2;
    return result;
}

bool TrianglePointerEqual::operator()(const Triangle* triangle0_,
                                      const Triangle* triangle1_) const
{
    unsigned int t0id0, t0id1, t0id2, t1id0, t1id1, t1id2;
    triangle0_->sortedIds(t0id0, t0id1, t0id2);
    triangle1_->sortedIds(t1id0, t1id1, t1id2);
    return (t0id0 == t1id0) && (t0id1 == t1id1) && (t0id2 == t1id2);
}

}  // namespace geometry
}  // namespace phyanim
