#ifndef __PHYANIM_PRIMITIVE__
#define __PHYANIM_PRIMITIVE__

#include <iostream>
#include <vector>

#include "Node.h"

namespace phyanim
{
namespace geometry
{
class Primitive;

typedef Primitive* PrimitivePtr;

typedef std::vector<PrimitivePtr> Primitives;

typedef std::pair<PrimitivePtr, PrimitivePtr> PrimitivePair;

typedef std::vector<PrimitivePair> PrimitivePairs;

class Primitive
{
public:
    Primitive() : _lowerLimit(maxVec3), _upperLimit(minVec3){};

    virtual ~Primitive(){};

    void update()
    {
        _lowerLimit = maxVec3;
        _upperLimit = minVec3;
        for (auto node : nodes())
        {
            Vec3 pos = node->position;
            float r = node->radius;
            // _lowerLimit.x() = std::min(pos.x - r, _lowerLimit.x());
            // _lowerLimit.y() = std::min(pos.y - r, _lowerLimit.y());
            // _lowerLimit.z() = std::min(pos.z - r, _lowerLimit.z());
            // _upperLimit.x() = std::max(pos.x + r, _upperLimit.x());
            // _upperLimit.y() = std::max(pos.y + r, _upperLimit.y());
            // _upperLimit.z() = std::max(pos.z + r, _upperLimit.z());

            _lowerLimit = glm::min(pos - r, _lowerLimit);
            _upperLimit = glm::max(pos + r, _upperLimit);
        }
    };

    Vec3 lowerLimit() const { return _lowerLimit; };

    Vec3 upperLimit() const { return _upperLimit; };

    Vec3 center() const { return (_lowerLimit + _upperLimit) * 0.5f; };

    virtual Nodes nodes() const = 0;

    bool areLimitsColliding(PrimitivePtr primitive) const
    {
        Vec3 thisLowerLimit = lowerLimit();
        Vec3 thisUpperLimit = upperLimit();
        Vec3 otherLowerLimit = primitive->lowerLimit();
        Vec3 otherUpperLimit = primitive->upperLimit();

        return (thisLowerLimit.x <= otherUpperLimit.x) &&
               (thisUpperLimit.x >= otherLowerLimit.x) &&
               (thisLowerLimit.y <= otherUpperLimit.y) &&
               (thisUpperLimit.y >= otherLowerLimit.y) &&
               (thisLowerLimit.z <= otherUpperLimit.z) &&
               (thisUpperLimit.z >= otherLowerLimit.z);
    };

    bool isSoma()
    {
        bool isSoma = true;
        for (auto node : nodes()) isSoma = isSoma && node->isSoma;
        return isSoma;
    }

protected:
    Vec3 _lowerLimit;

    Vec3 _upperLimit;
};

}  // namespace geometry
}  // namespace phyanim

#endif  // __PHYANIM_PRIMITIVE__