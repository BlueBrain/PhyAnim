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