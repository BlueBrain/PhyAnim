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

#ifndef __PHYANIM_HIERARCHICAL_AABB__
#define __PHYANIM_HIERARCHICAL_AABB__

#include "AxisAlignedBoundingBox.h"
#include "Edge.h"

namespace phyanim
{
namespace geometry
{
class HierarchicalAABB;

typedef HierarchicalAABB* HierarchicalAABBPtr;

typedef std::vector<HierarchicalAABBPtr> HierarchicalAABBs;

class HierarchicalAABB : public AxisAlignedBoundingBox
{
public:
    HierarchicalAABB();

    HierarchicalAABB(Primitives& primitives, uint64_t cellSize = 10);

    HierarchicalAABB(Edges& edges, uint64_t cellSize = 10);

    ~HierarchicalAABB();

    void update();

    Nodes outterNodes(const AxisAlignedBoundingBox& axisAlignedBoundingBox);

    Primitives insidePrimitives(
        const AxisAlignedBoundingBox& axisAlignedBoundingBox);
    Primitives collidingPrimitives(
        const AxisAlignedBoundingBox& axisAlignedBoundingBox);
    PrimitivePairs collidingPrimitives(HierarchicalAABBPtr hierarchicalAABB);

    Edges insideEdges(const AxisAlignedBoundingBox& axisAlignedBoundingBox);
    Edges collidingEdges(const AxisAlignedBoundingBox& axisAlignedBoundingBox);

protected:
    void _update();

    void _divide(Primitives& primitives, uint64_t cellSize);

    void _outterNodes(const AxisAlignedBoundingBox& aabb, Nodes& nodes);

    void _insidePrimitives(const AxisAlignedBoundingBox& axisAlignedBoundingBox,
                           Primitives& primitives);

    void _collidingPrimitives(
        const AxisAlignedBoundingBox& axisAlignedBoundingBox,
        Primitives& primitives);

    void _collidingPrimitives(HierarchicalAABBPtr aabb0,
                              HierarchicalAABBPtr aabb1,
                              PrimitivePairs& primitivePairs);

protected:
    Primitives _primitives;

    HierarchicalAABBPtr _child0;

    HierarchicalAABBPtr _child1;
};

}  // namespace geometry
}  // namespace phyanim

#endif  // __PHYANIM_HIERARCHICAL_AABB__