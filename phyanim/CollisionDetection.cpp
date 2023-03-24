#include "CollisionDetection.h"

#include <iostream>

namespace phyanim
{

uint32_t CollisionDetection::computeCollisions(HierarchicalAABBs& aabbs,
                                               double stiffness)
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
                _computeCollision(aabb0, aabb1, stiffness);
        }
    }

    uint32_t numCollisions = 0;
    for (auto collision : collisions) numCollisions += collision;

    return numCollisions;
}  // namespace phyanim

uint32_t CollisionDetection::computeSelfCollisions(HierarchicalAABBs& aabbs,
                                                   double stiffness)
{
    uint32_t numCollisions = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < aabbs.size(); ++i)
    {
        numCollisions += _computeCollision(aabbs[i], aabbs[i], stiffness);
    }
    return numCollisions;
}

uint32_t CollisionDetection::computeCollisions(HierarchicalAABBPtr aabb,
                                               double stiffness)
{
    return _computeCollision(aabb, aabb, stiffness);
}

bool CollisionDetection::computeCollisions(Meshes& meshes, double stiffness)
{
    HierarchicalAABBs aabbs;
    for (auto mesh : meshes) aabbs.push_back(mesh->boundingBox);
    return computeCollisions(aabbs, stiffness);
}

void CollisionDetection::computeCollisions(HierarchicalAABBs& aabbs,
                                           const AxisAlignedBoundingBox& aabb)
{
    for (auto aabb0 : aabbs)
    {
        auto nodes = aabb0->outterNodes(aabb);

        Vec3 lowerLimit = aabb.lowerLimit();
        Vec3 upperLimit = aabb.upperLimit();

        for (unsigned int i = 0; i < nodes.size(); i++)
        {
            auto node = nodes[i];
            bool collision = false;
            Vec3 pos = node->position;
            if (pos.x() <= lowerLimit.x())
            {
                pos.x() = lowerLimit.x();
                collision = true;
            }
            else if (pos.x() >= upperLimit.x())
            {
                pos.x() = upperLimit.x();
                collision = true;
            }
            if (pos.y() <= lowerLimit.y())
            {
                pos.y() = lowerLimit.y();
                collision = true;
            }
            else if (pos.y() >= upperLimit.y())
            {
                pos.y() = upperLimit.y();
                collision = true;
            }
            if (pos.z() <= lowerLimit.z())
            {
                pos.z() = lowerLimit.z();
                collision = true;
            }
            else if (pos.z() >= upperLimit.z())
            {
                pos.z() = upperLimit.z();
                collision = true;
            }

            if (collision)
            {
                node->position = pos;
                node->velocity = Vec3::Zero();
            }
        }
    }
}

void CollisionDetection::computeCollisions(Meshes& meshes,
                                           const AxisAlignedBoundingBox& aabb)
{
    HierarchicalAABBs aabbs;
    for (auto mesh : meshes) aabbs.push_back(mesh->boundingBox);
    computeCollisions(aabbs, aabb);
}

AxisAlignedBoundingBoxes CollisionDetection::collisionBoundingBoxes(
    HierarchicalAABBs& aabbs,
    double sizeFactor)
{
    AxisAlignedBoundingBoxes boundingBoxes;

    uint32_t numSomas = 0;
    std::unordered_set<PrimitivePtr> uPrims;

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
            auto bb =
                new AxisAlignedBoundingBox(p->lowerLimit(), p->upperLimit());
            bb->resize(sizeFactor);
            boundingBoxes.push_back(bb);
        }
        else
        {
            ++numSomas;
        }
    }
    // std::cout << "Num somas: " << numSomas << std::endl;
    // std::cout << "Num prim colliding: " << boundingBoxes.size() << std::endl;

    _mergeBoundingBoxes(boundingBoxes);

    return boundingBoxes;
}

AxisAlignedBoundingBoxes CollisionDetection::collisionBoundingBoxes(
    Meshes& meshes,
    double sizeFactor)
{
    HierarchicalAABBs aabbs;
    for (auto mesh : meshes) aabbs.push_back(mesh->boundingBox);
    return collisionBoundingBoxes(aabbs, sizeFactor);
}

uint32_t CollisionDetection::_computeCollision(HierarchicalAABBPtr aabb0,
                                               HierarchicalAABBPtr aabb1,
                                               double stiffness)
{
    uint32_t numCollisions = 0;
    auto pairs = aabb0->collidingPrimitives(aabb1);

    for (unsigned int i = 0; i < pairs.size(); i++)
    {
        auto pair = pairs[i];

        if (!pair.first->areLimitsColliding(pair.second)) continue;
        if (_checkCollision(pair.first, pair.second, stiffness))
            ++numCollisions;
    }
    return numCollisions;
}

bool CollisionDetection::_checkCollision(PrimitivePtr p0,
                                         PrimitivePtr p1,
                                         double stiffness,
                                         bool setForces)
{
    // auto t0 = dynamic_cast<TrianglePtr>(p0);
    // auto t1 = dynamic_cast<TrianglePtr>(p1);
    // if (t0 && t1)
    // {
    //     return _checkCollision(t0, t1, stiffness, setForces);
    // }
    // else
    {
        auto e0 = dynamic_cast<Edge*>(p0);
        auto e1 = dynamic_cast<Edge*>(p1);
        return _checkCollision(e0, e1, stiffness, setForces);
    }
    // return false;
}

bool CollisionDetection::_checkCollision(TrianglePtr t0,
                                         TrianglePtr t1,
                                         double stiffness,
                                         bool setForces)
{
    Vec3 vt0[3], vt1[3];
    vt0[0] = t0->node0->position;
    vt0[1] = t0->node1->position;
    vt0[2] = t0->node2->position;
    vt1[0] = t1->node0->position;
    vt1[1] = t1->node1->position;
    vt1[2] = t1->node2->position;

    Vec3 n0 = (vt0[1] - vt0[0]).cross(vt0[2] - vt0[0]);
    double d0 = -n0.dot(vt0[0]);

    double dt1[3];
    dt1[0] = n0.dot(vt1[0]) + d0;
    dt1[1] = n0.dot(vt1[1]) + d0;
    dt1[2] = n0.dot(vt1[2]) + d0;

    if ((dt1[0] > 0.0 && dt1[1] > 0.0 && dt1[2] > 0.0) ||
        (dt1[0] < 0.0 && dt1[1] < 0.0 && dt1[2] < 0.0))
    {
        return false;
    }

    Vec3 n1 = (vt1[1] - vt1[0]).cross(vt1[2] - vt1[0]);
    double d1 = -n1.dot(vt1[0]);

    double dt0[3];
    dt0[0] = n1.dot(vt0[0]) + d1;
    dt0[1] = n1.dot(vt0[1]) + d1;
    dt0[2] = n1.dot(vt0[2]) + d1;

    if ((dt0[0] > 0.0 && dt0[1] > 0.0 && dt0[2] > 0.0) ||
        (dt0[0] < 0.0 && dt0[1] < 0.0 && dt0[2] < 0.0))
    {
        return false;
    }

    Vec3 D = n0.cross(n1);

    double pt0[3], pt1[3];
    pt0[0] = D.dot(vt0[0]);
    pt0[1] = D.dot(vt0[1]);
    pt0[2] = D.dot(vt0[2]);
    pt1[0] = D.dot(vt1[0]);
    pt1[1] = D.dot(vt1[1]);
    pt1[2] = D.dot(vt1[2]);

    double sdt0[3], sdt1[3];
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

    double interval0[2], interval1[2];

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
        n0.normalize();
        n1.normalize();
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

bool CollisionDetection::_checkCollision(Edge* e0,
                                         Edge* e1,
                                         double stiffness,
                                         bool setForces)
{
    auto a = e0->node0->position;
    auto b = e0->node1->position;
    auto c = e1->node0->position;
    auto d = e1->node1->position;
    auto r0 = std::max(e0->node0->radius, e0->node1->radius);
    auto r1 = std::max(e1->node0->radius, e1->node1->radius);

    Vec3 p0, p1;
    double t0, t1;
    project(a, b, c, d, p0, t0, p1, t1);

    Vec3 dir = (p1 - p0);
    double dist = (dir.norm() - (r0 + r1));
    if (dist >= 0.0) return false;
    if (dist > -0.1) dist = -0.1;

    dir.normalize();
    auto f = dir * stiffness * dist;

    if (setForces)
    {
        e0->node0->force += f * (1 - t0);
        e0->node1->force += f * t0;
        e1->node0->force -= f * (1 - t1);
        e1->node1->force -= f * t1;
    }
    e0->node0->collide = true;
    e0->node1->collide = true;
    e1->node0->collide = true;
    e1->node1->collide = true;
    return true;
}

void CollisionDetection::_checkAndSetForce(NodePtr node_,
                                           Vec3 normal_,
                                           double dist_,
                                           double stiffness)
{
    if (dist_ < 0.0)
    {
        node_->force += -dist_ * stiffness * normal_;
    }
}

void CollisionDetection::_mergeBoundingBoxes(AxisAlignedBoundingBoxes& aabbs)
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

}  // namespace phyanim
