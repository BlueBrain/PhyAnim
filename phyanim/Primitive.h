#ifndef __PHYANIM_PRIMITIVE__
#define __PHYANIM_PRIMITIVE__

#include <iostream>
#include <vector>

#include "Edge.h"

namespace phyanim
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
            _lowerLimit.x() = std::min(pos.x(), _lowerLimit.x());
            _lowerLimit.y() = std::min(pos.y(), _lowerLimit.y());
            _lowerLimit.z() = std::min(pos.z(), _lowerLimit.z());
            _upperLimit.x() = std::max(pos.x(), _upperLimit.x());
            _upperLimit.y() = std::max(pos.y(), _upperLimit.y());
            _upperLimit.z() = std::max(pos.z(), _upperLimit.z());
        }
    };

    Vec3 lowerLimit() const { return _lowerLimit; };

    Vec3 upperLimit() const { return _upperLimit; };

    Vec3 center() const { return (_lowerLimit + _upperLimit) * 0.5; };

    virtual Nodes nodes() const = 0;

    virtual Edges edges() const = 0;

    bool areLimitsColliding(PrimitivePtr primitive) const
    {
        Vec3 thisLowerLimit = lowerLimit();
        Vec3 thisUpperLimit = upperLimit();
        Vec3 otherLowerLimit = primitive->lowerLimit();
        Vec3 otherUpperLimit = primitive->upperLimit();

        return (thisLowerLimit.x() <= otherUpperLimit.x()) &&
               (thisUpperLimit.x() >= otherLowerLimit.x()) &&
               (thisLowerLimit.y() <= otherUpperLimit.y()) &&
               (thisUpperLimit.y() >= otherLowerLimit.y()) &&
               (thisLowerLimit.z() <= otherUpperLimit.z()) &&
               (thisUpperLimit.z() >= otherLowerLimit.z());
    };

protected:
    Vec3 _lowerLimit;

    Vec3 _upperLimit;
};

}  // namespace phyanim

#endif  // __PHYANIM_PRIMITIVE__