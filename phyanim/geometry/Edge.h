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
