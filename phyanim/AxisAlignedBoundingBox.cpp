#include "AxisAlignedBoundingBox.h"

#include <iostream>
#include <limits>

namespace phyanim
{
AxisAlignedBoundingBox::AxisAlignedBoundingBox() { _clear(); }

AxisAlignedBoundingBox::AxisAlignedBoundingBox(Vec3 lowerLimit, Vec3 upperLimit)
    : _lowerLimit(lowerLimit)
    , _upperLimit(upperLimit)
{
}
AxisAlignedBoundingBox::AxisAlignedBoundingBox(Primitives primitives)
    : AxisAlignedBoundingBox()
{
    update(primitives);
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const AxisAlignedBoundingBox& other)
    : _lowerLimit(other.lowerLimit())
    , _upperLimit(other.upperLimit())
{
}

Vec3 AxisAlignedBoundingBox::lowerLimit() const { return _lowerLimit; }

Vec3 AxisAlignedBoundingBox::upperLimit() const { return _upperLimit; }

void AxisAlignedBoundingBox::lowerLimit(Vec3 lowerLimit)
{
    _lowerLimit = lowerLimit;
}

void AxisAlignedBoundingBox::upperLimit(Vec3 upperLimit)
{
    _upperLimit = upperLimit;
}

Vec3 AxisAlignedBoundingBox::center() const
{
    Vec3 result = (_lowerLimit + _upperLimit) * 0.5;
    return result;
}

float AxisAlignedBoundingBox::radius() const
{
    return (_upperLimit - center()).norm();
}

bool AxisAlignedBoundingBox::isColliding(const Primitive& primitive) const
{
    Vec3 lowerLimit = primitive.lowerLimit();
    Vec3 upperLimit = primitive.upperLimit();
    return _isColliding(lowerLimit, upperLimit);
}

bool AxisAlignedBoundingBox::isColliding(
    const AxisAlignedBoundingBox& other) const
{
    return _isColliding(other.lowerLimit(), other.upperLimit());
}

bool AxisAlignedBoundingBox::isInside(const Vec3& pos) const
{
    return _isInside(pos, pos);
}

bool AxisAlignedBoundingBox::isInside(const Primitive& primitive) const
{
    Vec3 lowerLimit = primitive.lowerLimit();
    Vec3 upperLimit = primitive.upperLimit();
    return _isInside(lowerLimit, upperLimit);
}

bool AxisAlignedBoundingBox::isInside(const AxisAlignedBoundingBox& other) const
{
    return _isInside(other.lowerLimit(), other.upperLimit());
}

void AxisAlignedBoundingBox::unite(const Vec3& pos) { _unite(pos, pos); }

void AxisAlignedBoundingBox::unite(const Primitives& primitives)
{
    for (auto primitive : primitives)
    {
        Vec3 lowerLimit = primitive->lowerLimit();
        Vec3 upperLimit = primitive->upperLimit();
        _unite(lowerLimit, upperLimit);
    }
}

void AxisAlignedBoundingBox::unite(const AxisAlignedBoundingBox& other)
{
    _unite(other.lowerLimit(), other.upperLimit());
}

void AxisAlignedBoundingBox::update(const Nodes& nodes)
{
    _clear();
    for (auto node : nodes)
    {
        unite(node->position);
    }
}

void AxisAlignedBoundingBox::update(const Primitives& primitives)
{
    _clear();
    for (auto primitive : primitives)
    {
        primitive->update();
    }
    unite(primitives);
}

void AxisAlignedBoundingBox::update(const AxisAlignedBoundingBox& other)
{
    _clear();
    unite(other);
}

void AxisAlignedBoundingBox::_clear()
{
    _lowerLimit = maxVec3;
    _upperLimit = minVec3;
}

bool AxisAlignedBoundingBox::_isColliding(const Vec3& lowerLimit,
                                          const Vec3& upperLimit) const
{
    return (_lowerLimit.x() <= upperLimit.x()) &&
           (_upperLimit.x() >= lowerLimit.x()) &&
           (_lowerLimit.y() <= upperLimit.y()) &&
           (_upperLimit.y() >= lowerLimit.y()) &&
           (_lowerLimit.z() <= upperLimit.z()) &&
           (_upperLimit.z() >= lowerLimit.z());
}
bool AxisAlignedBoundingBox::_isInside(const Vec3& lowerLimit,
                                       const Vec3& upperLimit) const
{
    return (_lowerLimit.x() <= lowerLimit.x()) &&
           (_upperLimit.x() >= upperLimit.x()) &&
           (_lowerLimit.y() <= lowerLimit.y()) &&
           (_upperLimit.y() >= upperLimit.y()) &&
           (_lowerLimit.z() <= lowerLimit.z()) &&
           (_upperLimit.z() >= upperLimit.z());
}

bool AxisAlignedBoundingBox::_unite(const Vec3& lowerLimit,
                                    const Vec3& upperLimit)
{
    _lowerLimit.x() = std::min(_lowerLimit.x(), lowerLimit.x());
    _lowerLimit.y() = std::min(_lowerLimit.y(), lowerLimit.y());
    _lowerLimit.z() = std::min(_lowerLimit.z(), lowerLimit.z());
    _upperLimit.x() = std::max(_upperLimit.x(), upperLimit.x());
    _upperLimit.y() = std::max(_upperLimit.y(), upperLimit.y());
    _upperLimit.z() = std::max(_upperLimit.z(), upperLimit.z());
}

}  // namespace phyanim
