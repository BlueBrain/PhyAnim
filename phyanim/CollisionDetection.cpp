#include "CollisionDetection.h"

#include <iostream>

namespace phyanim
{
const Vec3 CollisionDetection::_color = Vec3(1, 0.6, 0.6);

const Vec3 CollisionDetection::_collisionColor = Vec3(1, 0, 0);

bool CollisionDetection::computeCollisions(Meshes& dynamics,
                                           double stiffness,
                                           bool setColor)
{
    Meshes statics;
    return computeCollisions(dynamics, statics, stiffness, setColor);
}

bool CollisionDetection::computeCollisions(Meshes& dynamics,
                                           Meshes& statics,
                                           double stiffness,
                                           bool setColor)
{
    unsigned int size = dynamics.size();
    bool detectedCollision = false;
    bool (*checkMeshesCollision)(MeshPtr, MeshPtr, double);
    if (setColor)
    {
        checkMeshesCollision = _checkMeshesCollisionAndSetColor;
        for (auto mesh : dynamics)
        {
            for (auto node : mesh->nodes)
            {
                node->collide = false;
            }
        }
        for (auto mesh : statics)
        {
            for (auto node : mesh->nodes)
            {
                node->collide = false;
            }
        }
    }
    else
    {
        checkMeshesCollision = _checkMeshesCollision;
    }
    for (unsigned int i = 0; i < size; ++i)
    {
        auto dynamic0 = dynamics[i];
        for (unsigned int j = i + 1; j < size; ++j)
        {
            auto dynamic1 = dynamics[j];
            detectedCollision |=
                checkMeshesCollision(dynamic0, dynamic1, stiffness);
        }
        for (unsigned int j = 0; j < statics.size(); j++)
        {
            auto dynamic1 = statics[j];
            detectedCollision |=
                checkMeshesCollision(dynamic0, dynamic1, stiffness);
        }
    }
    return detectedCollision;
}

void CollisionDetection::computeCollisions(Meshes& meshes,
                                           const AxisAlignedBoundingBox& aabb)
{
    for (auto mesh : meshes)
    {
        auto nodes = mesh->boundingBox->outterNodes(aabb);

        Vec3 lowerLimit = aabb.lowerLimit();
        Vec3 upperLimit = aabb.upperLimit();
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
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

AxisAlignedBoundingBoxes CollisionDetection::collisionBoundingBoxes(
    Meshes& meshes,
    double sizeFactor)
{
    AxisAlignedBoundingBoxes boundingBoxes;

    for (uint64_t i = 0; i < meshes.size(); ++i)
    {
        for (uint64_t j = i + 1; j < meshes.size(); ++j)
        {
            auto mesh0 = meshes[i];
            auto mesh1 = meshes[j];
            auto trianglesPairs =
                mesh0->boundingBox->collidingPrimitives(mesh1->boundingBox);
            UniqueTriangles uTriangles;
            for (auto tPair : trianglesPairs)
            {
                auto t0 = dynamic_cast<TrianglePtr>(tPair.first);
                auto t1 = dynamic_cast<TrianglePtr>(tPair.second);
                if (_checkTrianglesCollision(t0, t1, 1000, false))
                {
                    uTriangles.insert(t0);
                    uTriangles.insert(t1);
                }
            }
            for (auto t : uTriangles)
            {
                auto bb = new AxisAlignedBoundingBox(t->lowerLimit(),
                                                     t->upperLimit());
                bb->resize(10);
                boundingBoxes.push_back(bb);
            }
        }
    }

    _mergeBoundingBoxes(boundingBoxes);

    // for (auto bb : boundingBoxes)
    // {
    //     bb->resize(sizeFactor);
    // }

    return boundingBoxes;
}

bool CollisionDetection::_checkMeshesCollision(MeshPtr mesh0,
                                               MeshPtr mesh1,
                                               double stiffness)
{
    bool detectedCollision = false;
    auto trianglePairs =
        mesh0->boundingBox->collidingPrimitives(mesh1->boundingBox);
#ifdef PHYANIM_USES_OPENMP
    omp_lock_t writelock;
    omp_init_lock(&writelock);
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < trianglePairs.size(); i++)
    {
        auto tPair = trianglePairs[i];
        bool collision = _checkTrianglesCollision(
            dynamic_cast<TrianglePtr>(tPair.first),
            dynamic_cast<TrianglePtr>(tPair.second), stiffness);
#ifdef PHYANIM_USES_OPENMP
        omp_set_lock(&writelock);
#endif
        detectedCollision |= collision;
#ifdef PHYANIM_USES_OPENMP
        omp_unset_lock(&writelock);
#endif
    }
#ifdef PHYANIM_USES_OPENMP
    omp_destroy_lock(&writelock);
#endif
    return detectedCollision;
}

bool CollisionDetection::_checkMeshesCollisionAndSetColor(MeshPtr mesh0,
                                                          MeshPtr mesh1,
                                                          double stiffness)
{
    bool detectedCollision = false;
    auto trianglePairs =
        mesh0->boundingBox->collidingPrimitives(mesh1->boundingBox);
#ifdef PHYANIM_USES_OPENMP
    omp_lock_t writelock;
    omp_init_lock(&writelock);
#pragma omp parallel for
#endif
    for (unsigned int i = 0; i < trianglePairs.size(); i++)
    {
        auto tPair = trianglePairs[i];
        bool collision = _checkTrianglesCollision(
            dynamic_cast<TrianglePtr>(tPair.first),
            dynamic_cast<TrianglePtr>(tPair.second), stiffness);
        if (collision)
        {
            for (auto node : tPair.first->nodes())
            {
                node->collide = true;
            }
            for (auto node : tPair.second->nodes())
            {
                node->collide = true;
            }
        }
#ifdef PHYANIM_USES_OPENMP
        omp_set_lock(&writelock);
#endif
        detectedCollision |= collision;
#ifdef PHYANIM_USES_OPENMP
        omp_unset_lock(&writelock);
#endif
    }
#ifdef PHYANIM_USES_OPENMP
    omp_destroy_lock(&writelock);
#endif
    return detectedCollision;
}

bool CollisionDetection::_checkTrianglesCollision(TrianglePtr t0_,
                                                  TrianglePtr t1_,
                                                  double stiffness,
                                                  bool setForces)
{
    Vec3 vt0[3], vt1[3];
    vt0[0] = t0_->node0->position;
    vt0[1] = t0_->node1->position;
    vt0[2] = t0_->node2->position;
    vt1[0] = t1_->node0->position;
    vt1[1] = t1_->node1->position;
    vt1[2] = t1_->node2->position;

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
        _checkAndSetForce(t0_->node0, n1, dt0[0], stiffness);
        _checkAndSetForce(t0_->node1, n1, dt0[1], stiffness);
        _checkAndSetForce(t0_->node2, n1, dt0[2], stiffness);
        _checkAndSetForce(t1_->node0, n0, dt1[0], stiffness);
        _checkAndSetForce(t1_->node1, n0, dt1[1], stiffness);
        _checkAndSetForce(t1_->node2, n0, dt1[2], stiffness);
    }
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
