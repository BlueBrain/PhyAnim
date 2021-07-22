#include "AxisAlignedBoundingBox.h"

#include <iostream>
#include <limits>

namespace phyanim
{
double const min = std::numeric_limits<double>::lowest();
double const max = std::numeric_limits<double>::max();
double const third = 1.0 / 3.0;

AABB::AABB() : lowerLimit(Vec3(max, max, max)), upperLimit(Vec3(min, min, min))
{
}

AABB::AABB(Vec3 lowerLimit_, Vec3 upperLimit_)
    : lowerLimit(lowerLimit_)
    , upperLimit(upperLimit_)
{
}

AABB::AABB(const AABB& other_)
    : lowerLimit(other_.lowerLimit)
    , upperLimit(other_.upperLimit)
{
}

AABB::~AABB() {}

bool AABB::collide(const AABB& other_) const
{
    return (lowerLimit.x() <= other_.upperLimit.x()) &&
           (upperLimit.x() >= other_.lowerLimit.x()) &&
           (lowerLimit.y() <= other_.upperLimit.y()) &&
           (upperLimit.y() >= other_.lowerLimit.y()) &&
           (lowerLimit.z() <= other_.upperLimit.z()) &&
           (upperLimit.z() >= other_.lowerLimit.z());
}

bool AABB::inside(const AABB& other_) const
{
    return (lowerLimit.x() <= other_.lowerLimit.x()) &&
           (upperLimit.x() >= other_.upperLimit.x()) &&
           (lowerLimit.y() <= other_.lowerLimit.y()) &&
           (upperLimit.y() >= other_.upperLimit.y()) &&
           (lowerLimit.z() <= other_.lowerLimit.z()) &&
           (upperLimit.z() >= other_.upperLimit.z());
}

bool AABB::inside(const Vec3& pos_) const
{
    return (lowerLimit.x() <= pos_.x()) && (upperLimit.x() >= pos_.x()) &&
           (lowerLimit.y() <= pos_.y()) && (upperLimit.y() >= pos_.y()) &&
           (lowerLimit.z() <= pos_.z()) && (upperLimit.z() >= pos_.z());
}

void AABB::update(const Vec3& pos_)
{
    lowerLimit.x() = std::min(lowerLimit.x(), pos_.x());
    lowerLimit.y() = std::min(lowerLimit.y(), pos_.y());
    lowerLimit.z() = std::min(lowerLimit.z(), pos_.z());
    upperLimit.x() = std::max(upperLimit.x(), pos_.x());
    upperLimit.y() = std::max(upperLimit.y(), pos_.y());
    upperLimit.z() = std::max(upperLimit.z(), pos_.z());
}

void AABB::update(const AABB& other_)
{
    lowerLimit.x() = std::min(lowerLimit.x(), other_.lowerLimit.x());
    lowerLimit.y() = std::min(lowerLimit.y(), other_.lowerLimit.y());
    lowerLimit.z() = std::min(lowerLimit.z(), other_.lowerLimit.z());
    upperLimit.x() = std::max(upperLimit.x(), other_.upperLimit.x());
    upperLimit.y() = std::max(upperLimit.y(), other_.upperLimit.y());
    upperLimit.z() = std::max(upperLimit.z(), other_.upperLimit.z());
}

Vec3 AABB::center() { return (lowerLimit + upperLimit) * 0.5; }

AABBNode::AABBNode() : aabb(), child0(nullptr), child1(nullptr) {}

AABBNode::AABBNode(AABBNode* other_)
    : aabb(other_->aabb)
    , child0(nullptr)
    , child1(nullptr)
{
}

AABBNode::~AABBNode()
{
    if (child0)
    {
        delete (child0);
    }
    if (child1)
    {
        delete (child1);
    }
}

void AABBNode::divide()
{
    if (triangles.size() <= 5)
    {
        return;
    }

    Vec3 axis = aabb.upperLimit - aabb.lowerLimit;
    Vec3 center = (aabb.upperLimit + aabb.lowerLimit) * 0.5;
    unsigned int divCoord;
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

    child0 = new AABBNode(this);
    child1 = new AABBNode(this);
    child0->aabb.upperLimit[divCoord] = center[divCoord];
    child1->aabb.lowerLimit[divCoord] = center[divCoord];
    for (unsigned int i = 0; i < triangles.size(); i++)
    {
        auto triangle = triangles[i];
        Vec3 tCenter = _center(triangle);
        if (tCenter[divCoord] <= center[divCoord])
        {
            child0->triangles.push_back(triangle);
        }
        else
        {
            child1->triangles.push_back(triangle);
        }
    }
    for (unsigned int i = 0; i < nodes.size(); i++)
    {
        auto node = nodes[i];
        if (node->position[divCoord] <= center[divCoord])
        {
            child0->nodes.push_back(node);
        }
        else
        {
            child1->nodes.push_back(node);
        }
    }

    child0->divide();
    child1->divide();
}

void AABBNode::outterNodes(Nodes& nodes_, const AABB& aabb_)
{
    if (child0 != nullptr && child1 != nullptr)
    {
        if (!aabb_.inside(child0->aabb))
        {
            child0->outterNodes(nodes_, aabb_);
        }
        if (!aabb_.inside(child1->aabb))
        {
            child1->outterNodes(nodes_, aabb_);
        }
    }
    else
    {
        for (unsigned int i = 0; i < nodes.size(); i++)
        {
            auto node = nodes[i];
            if (!aabb_.inside(node->position))
            {
                nodes_.push_back(node);
            }
        }
    }
}

void AABBNode::trianglePairs(TrianglePairs& trianglePairs_,
                             AABBNode* node0_,
                             AABBNode* node1_)
{
    if (node0_->aabb.collide(node1_->aabb))
    {
        if (node0_->child0 != nullptr && node0_->child1)
        {
            if (node1_->child0 != nullptr && node1_->child1)
            {
                trianglePairs(trianglePairs_, node0_->child0, node1_->child0);
                trianglePairs(trianglePairs_, node0_->child0, node1_->child1);
                trianglePairs(trianglePairs_, node0_->child1, node1_->child0);
                trianglePairs(trianglePairs_, node0_->child1, node1_->child1);
            }
            else
            {
                trianglePairs(trianglePairs_, node0_->child0, node1_);
                trianglePairs(trianglePairs_, node0_->child1, node1_);
            }
        }
        else
        {
            if (node1_->child0 != nullptr && node1_->child1)
            {
                trianglePairs(trianglePairs_, node0_, node1_->child0);
                trianglePairs(trianglePairs_, node0_, node1_->child1);
            }
            else
            {
                for (auto t0 : node0_->triangles)
                {
                    for (auto t1 : node1_->triangles)
                    {
                        trianglePairs_.push_back(std::make_pair(t0, t1));
                    }
                }
            }
        }
    }
}

void AABBNode::update()
{
    if (child0 != nullptr && child1 != nullptr)
    {
        child0->update();
        aabb.update(child0->aabb);
        child1->update();
        aabb.update(child1->aabb);
    }
    else
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (unsigned int i = 0; i < nodes.size(); i++)
        {
            auto node = nodes[i];
            aabb.update(node->position);
        }
    }
}

Vec3 AABBNode::_center(Triangle* triangle_)
{
    Vec3 center = (triangle_->node0->position + triangle_->node1->position +
                   triangle_->node2->position) *
                  0.333;
    return center;
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox() { root = new AABBNode(); }

AxisAlignedBoundingBox::AxisAlignedBoundingBox(Nodes& nodes_,
                                               Triangles& triangles_,
                                               Tetrahedra& tetrahedra_)
{
    root = new AABBNode();
    generate(nodes_, triangles_, tetrahedra_);
}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
    if (root)
    {
        delete (root);
    }
}

void AxisAlignedBoundingBox::generate(Nodes& nodes_,
                                      Triangles& triangles_,
                                      Tetrahedra& tetrahedra_)
{
    root->nodes = nodes_;
    root->triangles = triangles_;
    root->tetrahedra = tetrahedra_;
    for (auto tri : triangles_)
    {
        root->aabb.update(tri->node0->position);
        root->aabb.update(tri->node1->position);
        root->aabb.update(tri->node2->position);
    }
    root->divide();
}

void AxisAlignedBoundingBox::update() { root->update(); }

Nodes AxisAlignedBoundingBox::outterNodes(const AABB& aabb_)
{
    Nodes nodes;
    root->outterNodes(nodes, aabb_);
    return nodes;
}

TrianglePairs AxisAlignedBoundingBox::trianglePairs(
    AxisAlignedBoundingBox* other_)
{
    TrianglePairs trianglePairs;
    root->trianglePairs(trianglePairs, root, other_->root);
    return trianglePairs;
}

}  // namespace phyanim
