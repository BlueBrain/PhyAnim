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

#include "CollisionDetection.h"

#include <iostream>

namespace phyanim
{
namespace anim
{
uint32_t CollisionDetection::computeCollisions(
    geometry::HierarchicalAABBs& aabbs,
    float stiffness,
    float threshold)
{
    uint32_t size = aabbs.size();
    std::vector<uint32_t> collisions(size * size);
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < size; ++i)
    {
        auto aabb0 = aabbs[i];
        for (unsigned int j = i + 1; j < size; ++j)
        {
            auto aabb1 = aabbs[j];
            collisions[i * size + j] =
                _computeCollision(aabb0, aabb1, stiffness, threshold);
        }
    }

    uint32_t numCollisions = 0;
    for (auto collision : collisions) numCollisions += collision;

    return numCollisions;
}  // namespace phyanim

uint32_t CollisionDetection::computeSelfCollisions(
    geometry::HierarchicalAABBs& aabbs,
    float stiffness,
    float threshold)
{
    uint32_t numCollisions = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < aabbs.size(); ++i)
    {
        numCollisions +=
            _computeCollision(aabbs[i], aabbs[i], stiffness, threshold);
    }
    return numCollisions;
}

uint32_t CollisionDetection::computeCollisions(
    geometry::HierarchicalAABBPtr aabb,
    float stiffness,
    float threshold)
{
    return _computeCollision(aabb, aabb, stiffness, threshold);
}

bool CollisionDetection::computeCollisions(geometry::Meshes& meshes,
                                           float stiffness,
                                           float threshold)
{
    geometry::HierarchicalAABBs aabbs;
    for (auto mesh : meshes) aabbs.push_back(mesh->boundingBox);
    return computeCollisions(aabbs, stiffness, threshold);
}

void CollisionDetection::computeCollisions(
    geometry::HierarchicalAABBs& aabbs,
    const geometry::AxisAlignedBoundingBox& aabb)
{
    for (auto aabb0 : aabbs)
    {
        auto nodes = aabb0->outterNodes(aabb);

        geometry::Vec3 lowerLimit = aabb.lowerLimit();
        geometry::Vec3 upperLimit = aabb.upperLimit();

        for (unsigned int i = 0; i < nodes.size(); i++)
        {
            auto node = nodes[i];
            bool collision = false;
            geometry::Vec3 pos = node->position;
            if (pos.x <= lowerLimit.x)
            {
                pos.x = lowerLimit.x;
                collision = true;
            }
            else if (pos.x >= upperLimit.x)
            {
                pos.x = upperLimit.x;
                collision = true;
            }
            if (pos.y <= lowerLimit.y)
            {
                pos.y = lowerLimit.y;
                collision = true;
            }
            else if (pos.y >= upperLimit.y)
            {
                pos.y = upperLimit.y;
                collision = true;
            }
            if (pos.z <= lowerLimit.z)
            {
                pos.z = lowerLimit.z;
                collision = true;
            }
            else if (pos.z >= upperLimit.z)
            {
                pos.z = upperLimit.z;
                collision = true;
            }

            if (collision)
            {
                node->position = pos;
                node->velocity = geometry::Vec3();
            }
        }
    }
}

void CollisionDetection::computeCollisions(
    geometry::Meshes& meshes,
    const geometry::AxisAlignedBoundingBox& aabb)
{
    geometry::HierarchicalAABBs aabbs;
    for (auto mesh : meshes) aabbs.push_back(mesh->boundingBox);
    computeCollisions(aabbs, aabb);
}

geometry::AxisAlignedBoundingBoxes CollisionDetection::collisionBoundingBoxes(
    geometry::HierarchicalAABBs& aabbs,
    float sizeFactor)
{
    geometry::AxisAlignedBoundingBoxes boundingBoxes;

    uint32_t numSomas = 0;
    std::unordered_set<geometry::PrimitivePtr> uPrims;

    for (uint64_t i = 0; i < aabbs.size(); ++i)
    {
        for (uint64_t j = i + 1; j < aabbs.size(); ++j)
        {
            auto aabb0 = aabbs[i];
            auto aabb1 = aabbs[j];
            auto pairs = aabb0->collidingPrimitives(aabb1);

            for (auto pair : pairs)
            {
                if (_checkCollision(pair.first, pair.second, 0, false))
                {
                    uPrims.insert(pair.first);
                    uPrims.insert(pair.second);
                }
            }
        }
    }

    for (auto p : uPrims)
    {
        if (!p->isSoma())
        {
            auto bb = new geometry::AxisAlignedBoundingBox(p->lowerLimit(),
                                                           p->upperLimit());
            bb->resize(sizeFactor);
            boundingBoxes.push_back(bb);
        }
        else
        {
            ++numSomas;
        }
    }

    _mergeBoundingBoxes(boundingBoxes);

    return boundingBoxes;
}

geometry::AxisAlignedBoundingBoxes CollisionDetection::collisionBoundingBoxes(
    geometry::Meshes& meshes,
    float sizeFactor)
{
    geometry::HierarchicalAABBs aabbs;
    for (auto mesh : meshes) aabbs.push_back(mesh->boundingBox);
    return collisionBoundingBoxes(aabbs, sizeFactor);
}

uint32_t CollisionDetection::_computeCollision(
    geometry::HierarchicalAABBPtr aabb0,
    geometry::HierarchicalAABBPtr aabb1,
    float stiffness,
    float threshold)
{
    uint32_t numCollisions = 0;
    auto pairs = aabb0->collidingPrimitives(aabb1);

    for (unsigned int i = 0; i < pairs.size(); i++)
    {
        auto pair = pairs[i];

        if (!pair.first->areLimitsColliding(pair.second)) continue;
        if (_checkCollision(pair.first, pair.second, stiffness, threshold))
            ++numCollisions;
    }
    return numCollisions;
}

bool CollisionDetection::_checkCollision(geometry::PrimitivePtr p0,
                                         geometry::PrimitivePtr p1,
                                         float stiffness,
                                         float threshold,
                                         bool setForces)
{
    auto t0 = dynamic_cast<geometry::TrianglePtr>(p0);
    auto t1 = dynamic_cast<geometry::TrianglePtr>(p1);
    if (t0 && t1)
    {
        return _checkCollision(t0, t1, stiffness, setForces);
    }
    else
    {
        auto e0 = dynamic_cast<geometry::Edge*>(p0);
        auto e1 = dynamic_cast<geometry::Edge*>(p1);
        return _checkCollision(e0, e1, stiffness, threshold, setForces);
    }
    return false;
}

bool CollisionDetection::_checkCollision(geometry::TrianglePtr t0,
                                         geometry::TrianglePtr t1,
                                         float stiffness,
                                         bool setForces)
{
    geometry::Vec3 vt0[3], vt1[3];
    vt0[0] = t0->node0->position;
    vt0[1] = t0->node1->position;
    vt0[2] = t0->node2->position;
    vt1[0] = t1->node0->position;
    vt1[1] = t1->node1->position;
    vt1[2] = t1->node2->position;

    geometry::Vec3 n0 = glm::cross(vt0[1] - vt0[0], vt0[2] - vt0[0]);
    float d0 = glm::dot(-n0, vt0[0]);

    float dt1[3];
    dt1[0] = glm::dot(n0, vt1[0]) + d0;
    dt1[1] = glm::dot(n0, vt1[1]) + d0;
    dt1[2] = glm::dot(n0, vt1[2]) + d0;

    if ((dt1[0] > 0.0 && dt1[1] > 0.0 && dt1[2] > 0.0) ||
        (dt1[0] < 0.0 && dt1[1] < 0.0 && dt1[2] < 0.0))
    {
        return false;
    }

    geometry::Vec3 n1 = glm::cross(vt1[1] - vt1[0], vt1[2] - vt1[0]);
    float d1 = glm::dot(-n1, vt1[0]);

    float dt0[3];
    dt0[0] = glm::dot(n1, vt0[0]) + d1;
    dt0[1] = glm::dot(n1, vt0[1]) + d1;
    dt0[2] = glm::dot(n1, vt0[2]) + d1;

    if ((dt0[0] > 0.0 && dt0[1] > 0.0 && dt0[2] > 0.0) ||
        (dt0[0] < 0.0 && dt0[1] < 0.0 && dt0[2] < 0.0))
    {
        return false;
    }

    geometry::Vec3 D = glm::cross(n0, n1);

    float pt0[3], pt1[3];
    pt0[0] = glm::dot(D, vt0[0]);
    pt0[1] = glm::dot(D, vt0[1]);
    pt0[2] = glm::dot(D, vt0[2]);
    pt1[0] = glm::dot(D, vt1[0]);
    pt1[1] = glm::dot(D, vt1[1]);
    pt1[2] = glm::dot(D, vt1[2]);

    float sdt0[3], sdt1[3];
    sdt0[0] = (dt0[0] > 0) - (dt0[0] < 0);
    sdt0[1] = (dt0[1] > 0) - (dt0[1] < 0);
    sdt0[2] = (dt0[2] > 0) - (dt0[2] < 0);
    sdt1[0] = (dt1[0] > 0) - (dt1[0] < 0);
    sdt1[1] = (dt1[1] > 0) - (dt1[1] < 0);
    sdt1[2] = (dt1[2] > 0) - (dt1[2] < 0);

    unsigned int offt0 = 0;
    if (sdt0[1] != sdt0[2])
    {
        if (sdt0[0] == sdt0[2])
        {
            offt0 = 1;
        }
        else
        {
            offt0 = 2;
        }
    }

    unsigned int offt1 = 0;
    if (sdt1[1] != sdt1[2])
    {
        if (sdt1[0] == sdt1[2])
        {
            offt1 = 1;
        }
        else
        {
            offt1 = 2;
        }
    }

    float interval0[2], interval1[2];

    unsigned int t0id0 = offt0;
    unsigned int t0id1 = (offt0 + 1) % 3;
    unsigned int t0id2 = (offt0 + 2) % 3;

    unsigned int t1id0 = offt1;
    unsigned int t1id1 = (offt1 + 1) % 3;
    unsigned int t1id2 = (offt1 + 2) % 3;

    interval0[0] = pt0[t0id1] + (pt0[t0id0] - pt0[t0id1]) * dt0[t0id1] /
                                    (dt0[t0id1] - dt0[t0id0]);
    interval0[1] = pt0[t0id2] + (pt0[t0id0] - pt0[t0id2]) * dt0[t0id2] /
                                    (dt0[t0id2] - dt0[t0id0]);

    interval1[0] = pt1[t1id1] + (pt1[t1id0] - pt1[t1id1]) * dt1[t1id1] /
                                    (dt1[t1id1] - dt1[t1id0]);
    interval1[1] = pt1[t1id2] + (pt1[t1id0] - pt1[t1id2]) * dt1[t1id2] /
                                    (dt1[t1id2] - dt1[t1id0]);

    if (interval0[0] > interval0[1])
    {
        std::swap(interval0[0], interval0[1]);
    }
    if (interval1[0] > interval1[1])
    {
        std::swap(interval1[0], interval1[1]);
    }
    if (interval0[1] < interval1[0])
    {
        return false;
    }
    if (interval1[1] < interval0[0])
    {
        return false;
    }

    if (setForces)
    {
        n0 = glm::normalize(n0);
        n1 = glm::normalize(n1);
        _checkAndSetForce(t0->node0, n1, dt0[0], stiffness);
        _checkAndSetForce(t0->node1, n1, dt0[1], stiffness);
        _checkAndSetForce(t0->node2, n1, dt0[2], stiffness);
        _checkAndSetForce(t1->node0, n0, dt1[0], stiffness);
        _checkAndSetForce(t1->node1, n0, dt1[1], stiffness);
        _checkAndSetForce(t1->node2, n0, dt1[2], stiffness);
    }

    t0->node0->collide = true;
    t0->node1->collide = true;
    t0->node2->collide = true;
    t1->node0->collide = true;
    t1->node1->collide = true;
    t1->node2->collide = true;
    return true;
}

bool CollisionDetection::_checkCollision(geometry::Edge* e0,
                                         geometry::Edge* e1,
                                         float stiffness,
                                         float threshold,
                                         bool setForces)
{
    auto a = e0->node0->position;
    auto b = e0->node1->position;
    auto c = e1->node0->position;
    auto d = e1->node1->position;
    auto r0 = std::max(e0->node0->radius, e0->node1->radius);
    auto r1 = std::max(e1->node0->radius, e1->node1->radius);

    geometry::Vec3 p0, p1;
    float t0, t1;
    geometry::project(a, b, c, d, p0, t0, p1, t1);

    float dis = glm::distance(p1, p0) - (r0 + r1);
    if (dis >= 0.0f) return false;
    if (dis > -threshold) dis = -threshold;

    auto dir = glm::normalize(p1 - p0);
    auto f = dir * stiffness * dis;

    if (setForces)
    {
        e0->node0->force += f * (1.0f - t0);
        e0->node1->force += f * t0;
        e1->node0->force -= f * (1.0f - t1);
        e1->node1->force -= f * t1;
    }
    e0->node0->collide = true;
    e0->node1->collide = true;
    e1->node0->collide = true;
    e1->node1->collide = true;
    return true;
}

void CollisionDetection::_checkAndSetForce(geometry::NodePtr node_,
                                           geometry::Vec3 normal_,
                                           float dist_,
                                           float stiffness)
{
    if (dist_ < 0.0)
    {
        node_->force += -dist_ * stiffness * normal_;
    }
}

void CollisionDetection::_mergeBoundingBoxes(
    geometry::AxisAlignedBoundingBoxes& aabbs)
{
    bool merge = true;
    while (merge)
    {
        merge = false;
        for (uint64_t i = 0; i < aabbs.size(); ++i)
        {
            auto aabb0 = aabbs[i];
            for (uint64_t j = i + 1; j < aabbs.size(); ++j)
            {
                auto aabb1 = aabbs[j];
                if (aabb0->isColliding(*aabb1))
                {
                    merge = true;
                    aabb0->unite(*aabb1);
                    delete aabb1;
                    aabbs.erase(aabbs.begin() + j);
                    --j;
                }
            }
        }
    }
}

}  // namespace anim
}  // namespace phyanim
