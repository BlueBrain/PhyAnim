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

#include "Edge.h"

namespace phyanim
{
namespace geometry
{
Edge::Edge(Node* node0_, Node* node1_) : node0(node0_), node1(node1_)
{
    update();
    resLength = glm::distance(node1->position, node0->position);
}

Edge::~Edge() {}

void Edge::computeLen()
{
    resLength = glm::distance(node1->position, node0->position);
};

bool Edge::operator==(const Edge& edge_) const
{
    unsigned int id0 = node0->id;
    unsigned int id1 = node1->id;
    unsigned int oId0 = edge_.node0->id;
    unsigned int oId1 = edge_.node1->id;
    if (id1 > id0)
    {
        std::swap(id1, id0);
    }
    if (oId1 > oId0)
    {
        std::swap(oId1, oId0);
    }
    return (id0 == oId0) & (id1 == oId1);
}

size_t EdgePointerHash::operator()(const Edge* edge_) const
{
    unsigned int id0 = edge_->node0->id;
    unsigned int id1 = edge_->node1->id;
    if (id1 > id0)
    {
        std::swap(id1, id0);
    }
    return std::hash<unsigned int>{}(id0) ^ std::hash<unsigned int>{}(id1);
}

bool EdgePointerEqual::operator()(const Edge* edge0_, const Edge* edge1_) const
{
    unsigned int e0id0 = edge0_->node0->id;
    unsigned int e0id1 = edge0_->node1->id;
    unsigned int e1id0 = edge1_->node0->id;
    unsigned int e1id1 = edge1_->node1->id;
    if (e0id1 > e0id0)
    {
        std::swap(e0id0, e0id1);
    }
    if (e1id1 > e1id0)
    {
        std::swap(e1id0, e1id1);
    }
    return (e0id0 == e1id0) && (e0id1 == e1id1);
}
void computeLens(Edges& edges)
{
    for (auto edge : edges) edge->computeLen();
}

Nodes uniqueNodes(Edges& edges)
{
    std::unordered_set<NodePtr> uNodes;
    for (auto edge : edges)
        for (auto node : edge->nodes()) uNodes.insert(node);
    return Nodes(uNodes.begin(), uNodes.end());
}

void resample(Edges& edges)
{
    Edges newEdges;
    for (auto edge : edges)
    {
        auto r0 = edge->node0->radius;
        auto r1 = edge->node1->radius;
        auto pos0 = edge->node0->position;
        auto pos1 = edge->node1->position;

        auto dist = glm::distance(pos1, pos0);
        uint32_t numSides = floor(dist / (std::max(r0, r1)));

        if (numSides > 1)
        {
            float tInc = 1.0 / numSides;

            auto prevNode = edge->node0;
            for (uint32_t i = 1; i < numSides; ++i)
            {
                float t = tInc * i;
                Vec3 pos(pos0 * (1.0f - t) + pos1 * t);
                float r = r0 * (1.0 - t) + r1 * t;
                auto node = new Node(pos, 0, r, Vec3(), Vec3(), r);
                newEdges.push_back(new Edge(prevNode, node));
                prevNode = node;
            }
            newEdges.push_back(new Edge(prevNode, edge->node1));
            delete edge;
        }
        else
        {
            newEdges.push_back(edge);
        }
    }
    edges.clear();
    edges.insert(edges.end(), newEdges.begin(), newEdges.end());
}

void resample(Edges& edges, float len)
{
    Edges newEdges;
    for (auto edge : edges)
    {
        auto r0 = edge->node0->radius;
        auto r1 = edge->node1->radius;
        auto pos0 = edge->node0->position;
        auto pos1 = edge->node1->position;

        auto dist = glm::distance(pos1, pos0);
        uint32_t numSides = floor(dist / len);

        if (numSides > 1)
        {
            float tInc = 1.0 / numSides;

            auto prevNode = edge->node0;
            for (uint32_t i = 1; i < numSides; ++i)
            {
                float t = tInc * i;
                Vec3 pos(pos0 * (1.0f - t) + pos1 * t);
                float r = r0 * (1.0 - t) + r1 * t;
                auto node = new Node(pos, 0, r, Vec3(), Vec3(), r);
                newEdges.push_back(new Edge(prevNode, node));
                prevNode = node;
            }
            newEdges.push_back(new Edge(prevNode, edge->node1));
            delete edge;
        }
        else
        {
            newEdges.push_back(edge);
        }
    }
    edges.clear();
    edges.insert(edges.end(), newEdges.begin(), newEdges.end());
}

void removeOutEdges(Edges& edges, AxisAlignedBoundingBox& limits)
{
    for (uint32_t i = 0; i < edges.size(); ++i)
    {
        auto node0 = edges[i]->node0;
        auto node1 = edges[i]->node1;
        if (!node0->isSoma && !node1->isSoma &&
            (!limits.isInside(node0->position) ||
             !limits.isInside(node1->position)))
        {
            edges.erase(edges.begin() + i);
            --i;
        }
    }
}

}  // namespace geometry
}  // namespace phyanim
