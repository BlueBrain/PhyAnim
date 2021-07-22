#include <BoundingSphere.h>

namespace phyanim
{
BoundingSphere::BoundingSphere() : center(Vec3::Zero()), radius(0.0) {}

BoundingSphere::~BoundingSphere() {}

void BoundingSphere::update(Nodes& nodes_)
{
    center = Vec3::Zero();
    radius = 0.0;
    if (nodes_.size() > 0)
    {
        for (auto node : nodes_)
        {
            center += node->position;
        }
        center /= nodes_.size();
        for (auto node : nodes_)
        {
            double tempRadius = (center - node->position).norm();
            if (tempRadius > radius)
            {
                radius = tempRadius;
            }
        }
    }
}

bool BoundingSphere::checkCollision(BoundingVolume* other_)
{
    auto bSphere = dynamic_cast<BoundingSphere*>(other_);
    if (!bSphere) return false;
    return (center - bSphere->center).norm() <= (radius + bSphere->radius);
}

bool BoundingSphere::checkLimitsCollision(const Vec3& lowerLimit_,
                                          const Vec3& upperLimit_)
{
    Vec3 radiusVec(radius, radius, radius);
    Vec3 lowerBounding = center - radiusVec;
    if ((lowerBounding.x() <= lowerLimit_.x()) ||
        (lowerBounding.y() <= lowerLimit_.y()) ||
        (lowerBounding.z() <= lowerLimit_.z()))
    {
        return true;
    }
    Vec3 upperBounding = center + radiusVec;
    if ((upperBounding.x() >= upperLimit_.x()) ||
        (upperBounding.y() >= upperLimit_.y()) ||
        (upperBounding.z() >= upperLimit_.z()))
    {
        return true;
    }
    return false;
}

}  // namespace phyanim
