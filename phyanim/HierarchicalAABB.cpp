#include "HierarchicalAABB.h"

#include <iostream>

namespace phyanim
{
HierarchicalAABB::HierarchicalAABB(Primitives& primitives, uint64_t cellSize)
    : _child0(nullptr)
    , _child1(nullptr)
{
    _divide(primitives, cellSize);
}

HierarchicalAABB::HierarchicalAABB(Edges& edges, uint64_t cellSize)
    : _child0(nullptr)
    , _child1(nullptr)
{
    Primitives primitives(edges.begin(), edges.end());
    _divide(primitives, cellSize);
}

HierarchicalAABB::~HierarchicalAABB()
{
    if (_child0)
    {
        delete _child0;
    }
    if (_child1)
    {
        delete _child1;
    }
    _primitives.clear();
}

void HierarchicalAABB::update() { _update(); }

Nodes HierarchicalAABB::outterNodes(
    const AxisAlignedBoundingBox& axisAlignedBoundingBox)
{
    Nodes nodes;
    _outterNodes(axisAlignedBoundingBox, nodes);
    return nodes;
}

Primitives HierarchicalAABB::insidePrimitives(
    const AxisAlignedBoundingBox& axisAlignedBoundingBox)
{
    Primitives primitives;
    _insidePrimitives(axisAlignedBoundingBox, primitives);
    return primitives;
}

Primitives HierarchicalAABB::collidingPrimitives(
    const AxisAlignedBoundingBox& axisAlignedBoundingBox)
{
    Primitives primitives;
    _collidingPrimitives(axisAlignedBoundingBox, primitives);
    return primitives;
}

PrimitivePairs HierarchicalAABB::collidingPrimitives(
    HierarchicalAABBPtr hierarchicalAABB)
{
    PrimitivePairs primitivePairs;
    _collidingPrimitives(this, hierarchicalAABB, primitivePairs);
    return primitivePairs;
}

Edges HierarchicalAABB::insideEdges(
    const AxisAlignedBoundingBox& axisAlignedBoundingBox)
{
    Primitives primitives;
    _insidePrimitives(axisAlignedBoundingBox, primitives);
    Edges edges;
    for (auto prim : primitives)
        if (auto edge = dynamic_cast<Edge*>(prim)) edges.push_back(edge);
    return edges;
}

Edges HierarchicalAABB::collidingEdges(
    const AxisAlignedBoundingBox& axisAlignedBoundingBox)
{
    Primitives primitives;
    _collidingPrimitives(axisAlignedBoundingBox, primitives);
    Edges edges;
    for (auto prim : primitives)
        if (auto edge = dynamic_cast<Edge*>(prim)) edges.push_back(edge);
    return edges;
}

void HierarchicalAABB::_update()
{
    if (_primitives.size() > 0)
    {
        this->AxisAlignedBoundingBox::update(_primitives);
        for (auto primitive : _primitives)
        {
            primitive->update();
        }
    }
    else
    {
        _clear();
        if (_child0)
        {
            _child0->_update();
            unite(*_child0);
        }
        if (_child1)
        {
            _child1->_update();
            unite(*_child1);
        }
    }
}

void HierarchicalAABB::_divide(Primitives& primitives, uint64_t cellSize)
{
    this->AxisAlignedBoundingBox::update(primitives);
    if (primitives.size() <= cellSize)
    {
        _primitives = primitives;
        return;
    }
    else
    {
        Vec3 axis = _upperLimit - _lowerLimit;
        Vec3 center = this->center();
        uint8_t divCoord = 0;
        Primitives child0Primitives;
        Primitives child1Primitives;
        if (axis.x() >= axis.y() && axis.x() >= axis.z())
        {
            divCoord = 0;
        }
        else if (axis.y() >= axis.z())
        {
            divCoord = 1;
        }
        else
        {
            divCoord = 2;
        }
        for (auto primitive : primitives)
        {
            if (primitive->center()[divCoord] <= center[divCoord])
            {
                child0Primitives.push_back(primitive);
            }
            else
            {
                child1Primitives.push_back(primitive);
            }
        }

        if (child0Primitives.size() > 0 && child1Primitives.size() > 0)
        {
            _child0 = new HierarchicalAABB(child0Primitives, cellSize);
            _child1 = new HierarchicalAABB(child1Primitives, cellSize);

            this->AxisAlignedBoundingBox::update(*_child0);
            this->unite(*_child1);
        }
        else
        {
            _primitives = primitives;
        }
    }
}

void HierarchicalAABB::_outterNodes(const AxisAlignedBoundingBox& aabb,
                                    Nodes& nodes)
{
    if (_child0 && _child1)
    {
        if (!aabb.isInside(*_child0))
        {
            _child0->_outterNodes(aabb, nodes);
        }
        if (!aabb.isInside(*_child1))
        {
            _child1->_outterNodes(aabb, nodes);
        }
    }
    else
    {
        for (auto primitive : _primitives)
        {
            for (auto node : primitive->nodes())
            {
                if (!aabb.isInside(node->position))
                {
                    nodes.push_back(node);
                }
            }
        }
    }
}

void HierarchicalAABB::_insidePrimitives(const AxisAlignedBoundingBox& aabb,
                                         Primitives& primitives)
{
    if (_child0 && _child1)
    {
        if (aabb.isColliding(*_child0))
        {
            _child0->_insidePrimitives(aabb, primitives);
        }
        if (aabb.isColliding(*_child1))
        {
            _child1->_insidePrimitives(aabb, primitives);
        }
    }
    else
    {
        for (auto primitive : _primitives)
        {
            if (aabb.isInside(*primitive))
            {
                primitives.push_back(primitive);
            }
        }
    }
}

void HierarchicalAABB::_collidingPrimitives(const AxisAlignedBoundingBox& aabb,
                                            Primitives& primitives)
{
    if (_child0 && _child1)
    {
        if (aabb.isColliding(*_child0))
        {
            _child0->_collidingPrimitives(aabb, primitives);
        }
        if (aabb.isColliding(*_child1))
        {
            _child1->_collidingPrimitives(aabb, primitives);
        }
    }
    else
    {
        for (auto primitive : _primitives)
        {
            if (aabb.isColliding(*primitive))
            {
                primitives.push_back(primitive);
            }
        }
    }
}

void HierarchicalAABB::_collidingPrimitives(HierarchicalAABBPtr aabb0,
                                            HierarchicalAABBPtr aabb1,
                                            PrimitivePairs& primitivesPairs)
{
    if (aabb0->isColliding(*aabb1))
    {
        if (aabb0->_child0 && aabb0->_child1)
        {
            if (aabb1->_child0 && aabb1->_child1)
            {
                _collidingPrimitives(aabb0->_child0, aabb1->_child0,
                                     primitivesPairs);
                _collidingPrimitives(aabb0->_child0, aabb1->_child1,
                                     primitivesPairs);
                _collidingPrimitives(aabb0->_child1, aabb1->_child0,
                                     primitivesPairs);
                _collidingPrimitives(aabb0->_child1, aabb1->_child1,
                                     primitivesPairs);
            }
            else
            {
                _collidingPrimitives(aabb0->_child0, aabb1, primitivesPairs);
                _collidingPrimitives(aabb0->_child1, aabb1, primitivesPairs);
            }
        }
        else
        {
            if (aabb1->_child0 && aabb1->_child1)
            {
                _collidingPrimitives(aabb0, aabb1->_child0, primitivesPairs);
                _collidingPrimitives(aabb0, aabb1->_child1, primitivesPairs);
            }
            else
            {
                for (auto prim0 : aabb0->_primitives)
                {
                    for (auto prim1 : aabb1->_primitives)
                    {
                        if (prim0->areLimitsColliding(prim1))
                        {
                            primitivesPairs.push_back(
                                std::make_pair(prim0, prim1));
                        }
                    }
                }
            }
        }
    }
}

}  // namespace phyanim