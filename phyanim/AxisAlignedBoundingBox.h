
#ifndef __PHYANIM_AXIS_ALIGNED_BOUNDING_BOX__
#define __PHYANIM_AXIS_ALIGNED_BOUNDING_BOX__

#include "Math.h"

namespace phyanim
{
class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox(void);

    AxisAlignedBoundingBox(Vec3 lowerLimit, Vec3 upperLimit);

    AxisAlignedBoundingBox(const AxisAlignedBoundingBox& other);

    ~AxisAlignedBoundingBox();

    bool isColliding(const AxisAlignedBoundingBox& other) const;

    bool isInside(const AxisAlignedBoundingBox& other) const;

    bool isInside(const Vec3& pos) const;

    void unite(const AxisAlignedBoundingBox& other_);

    void unite(const Vec3& pos_);

    Vec3 lowerLimit() const;

    Vec3 upperLimit() const;

    Vec3 center() const;

    float radius() const;

protected:
    Vec3 _lowerLimit;

    Vec3 _upperLimit;
};

}  // namespace phyanim

#endif  // __PHYANIM_AXIS_ALIGNED_BOUNDING_BOX__
