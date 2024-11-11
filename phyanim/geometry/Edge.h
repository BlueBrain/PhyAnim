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

#ifndef __PHYANIM_EDGE__
#define __PHYANIM_EDGE__

#include <unordered_set>

#include "AxisAlignedBoundingBox.h"

namespace phyanim
{
namespace geometry
{
class Edge;

typedef std::vector<Edge*> Edges;

struct EdgePointerHash
{
public:
    size_t operator()(const Edge* edge_) const;
};

struct EdgePointerEqual
{
public:
    bool operator()(const Edge* edge0_, const Edge* edge1_) const;
};

typedef std::unordered_set<Edge*, EdgePointerHash, EdgePointerEqual>
    UniqueEdges;

class Edge : public Primitive
{
public:
    Edge(Node* node0_, Node* node1_);

    virtual ~Edge(void);

    Nodes nodes() const
    {
        Nodes nodes(2);
        nodes[0] = node0;
        nodes[1] = node1;
        return nodes;
    };

    Edges edges() const
    {
        Edges edges(1);
        edges[0] = new Edge(node0, node1);
        return edges;
    };

    void computeLen();

    bool operator==(const Edge& edge_) const;

    Node* node0;

    Node* node1;

    float resLength;
};

void computeLens(Edges& edges);

Nodes uniqueNodes(Edges& edges);

void resample(Edges& edges);

void resample(Edges& edges, float len);

void removeOutEdges(Edges& edges, AxisAlignedBoundingBox& limits);

}  // namespace geometry
}  // namespace phyanim

#endif
