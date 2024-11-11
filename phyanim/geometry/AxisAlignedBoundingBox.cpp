#include "AxisAlignedBoundingBox.h"

#include <iostream>
#include <limits>

namespace phyanim
{
namespace geometry
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
    Vec3 result = (_lowerLimit + _upperLimit) * 0.5f;
    return result;
}

float AxisAlignedBoundingBox::radius() const
{
    return glm::distance(_upperLimit, center());
}

bool AxisAlignedBoundingBox::isColliding(const Node& node) const
{
    Vec3 r(node.radius, node.radius, node.radius);
    Vec3 lowerLimit = node.position - r;
    Vec3 upperLimit = node.position + r;
    return _isColliding(lowerLimit, upperLimit);
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

void AxisAlignedBoundingBox::unite(const Vec3& lowerLimit,
                                   const Vec3& upperLimit)
{
    // _lowerLimit.x() = std::min(_lowerLimit.x(), lowerLimit.x());
    // _lowerLimit.y() = std::min(_lowerLimit.y(), lowerLimit.y());
    // _lowerLimit.z() = std::min(_lowerLimit.z(), lowerLimit.z());
    // _upperLimit.x() = std::max(_upperLimit.x(), upperLimit.x());
    // _upperLimit.y() = std::max(_upperLimit.y(), upperLimit.y());
    // _upperLimit.z() = std::max(_upperLimit.z(), upperLimit.z());

    _lowerLimit = glm::min(_lowerLimit, lowerLimit);
    _upperLimit = glm::max(_upperLimit, upperLimit);
}

void AxisAlignedBoundingBox::unite(const Vec3& pos) { unite(pos, pos); }

void AxisAlignedBoundingBox::unite(const Primitives& primitives)
{
    for (auto primitive : primitives)
    {
        Vec3 lowerLimit = primitive->lowerLimit();
        Vec3 upperLimit = primitive->upperLimit();
        unite(lowerLimit, upperLimit);
    }
}

void AxisAlignedBoundingBox::unite(const AxisAlignedBoundingBox& other)
{
    unite(other.lowerLimit(), other.upperLimit());
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

void AxisAlignedBoundingBox::resize(float resizeFactor)
{
    if (_lowerLimit != maxVec3 && _upperLimit != minVec3)
    {
        Vec3 center = this->center();
        Vec3 axis = _upperLimit - center;

        _lowerLimit -= axis * resizeFactor;
        _upperLimit += axis * resizeFactor;
    }
}

void AxisAlignedBoundingBox::fixOutNodes(Nodes& nodes)
{
    for (auto node : nodes) node->fix = !isInside(node->position);
}

void AxisAlignedBoundingBox::delimit(Nodes& nodes)
{
    uint32_t size = nodes.size();

    for (uint32_t i = 0; i < size; ++i)
    {
        auto node = nodes[i];
        if (!node->fix && !node->isSoma)
        {
            Vec3& pos = node->position;
            if (pos.x < _lowerLimit.x) pos.x = _lowerLimit.x;
            if (pos.y < _lowerLimit.y) pos.y = _lowerLimit.y;
            if (pos.z < _lowerLimit.z) pos.z = _lowerLimit.z;
            if (pos.x > _upperLimit.x) pos.x = _upperLimit.x;
            if (pos.y > _upperLimit.y) pos.y = _upperLimit.y;
            if (pos.z > _upperLimit.z) pos.z = _upperLimit.z;
        }
    }
}

void AxisAlignedBoundingBox::delimitIfCollide(Nodes& nodes)
{
    uint32_t size = nodes.size();

    for (uint32_t i = 0; i < size; ++i)
    {
        auto node = nodes[i];
        if (!node->fix && !node->isSoma && node->collide)
        {
            Vec3& pos = node->position;
            if (pos.x < _lowerLimit.x) pos.x = _lowerLimit.x;
            if (pos.y < _lowerLimit.y) pos.y = _lowerLimit.y;
            if (pos.z < _lowerLimit.z) pos.z = _lowerLimit.z;
            if (pos.x > _upperLimit.x) pos.x = _upperLimit.x;
            if (pos.y > _upperLimit.y) pos.y = _upperLimit.y;
            if (pos.z > _upperLimit.z) pos.z = _upperLimit.z;
        }
    }
}

void AxisAlignedBoundingBox::_clear()
{
    _lowerLimit = maxVec3;
    _upperLimit = minVec3;
}

bool AxisAlignedBoundingBox::_isColliding(const Vec3& lowerLimit,
                                          const Vec3& upperLimit) const
{
    return (_lowerLimit.x <= upperLimit.x) && (_upperLimit.x >= lowerLimit.x) &&
           (_lowerLimit.y <= upperLimit.y) && (_upperLimit.y >= lowerLimit.y) &&
           (_lowerLimit.z <= upperLimit.z) && (_upperLimit.z >= lowerLimit.z);
}
bool AxisAlignedBoundingBox::_isInside(const Vec3& lowerLimit,
                                       const Vec3& upperLimit) const
{
    return (_lowerLimit.x <= lowerLimit.x) && (_upperLimit.x >= upperLimit.x) &&
           (_lowerLimit.y <= lowerLimit.y) && (_upperLimit.y >= upperLimit.y) &&
           (_lowerLimit.z <= lowerLimit.z) && (_upperLimit.z >= upperLimit.z);
}

}  // namespace geometry
}  // namespace phyanim
