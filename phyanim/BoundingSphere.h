#ifndef __PHYANIM_BOUNDINGSPHERE__
#define __PHYANIM_BOUNDINGSPHERE__

#include <BoundingVolume.h>

namespace phyanim
{
class BoundingSphere : public BoundingVolume
{
public:
    BoundingSphere(void);

    virtual ~BoundingSphere(void);

    void update(Nodes& nodes_);

    bool checkCollision(BoundingVolume* other_);

    bool checkLimitsCollision(const Vec3& lowerLimit_, const Vec3& upperLimit_);

    Vec3 center;

    double radius;
};

}  // namespace phyanim

#endif
