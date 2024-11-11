
#ifndef __PHYANIM_AXIS_ALIGNED_BOUNDING_BOX__
#define __PHYANIM_AXIS_ALIGNED_BOUNDING_BOX__

#include <set>

#include "Primitive.h"

namespace phyanim
{
namespace geometry
{
class AxisAlignedBoundingBox;

typedef AxisAlignedBoundingBox* AxisAlignedBoundingBoxPtr;

typedef std::vector<AxisAlignedBoundingBoxPtr> AxisAlignedBoundingBoxes;

class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox();
    AxisAlignedBoundingBox(Vec3 lowerLimit, Vec3 upperLimit);
    AxisAlignedBoundingBox(Primitives primitives);
    AxisAlignedBoundingBox(const AxisAlignedBoundingBox& other);

    Vec3 lowerLimit() const;

    Vec3 upperLimit() const;

    void lowerLimit(Vec3 lowerLimit);

    void upperLimit(Vec3 upperLimit);

    Vec3 center() const;

    float radius() const;

    bool isColliding(const Node& node) const;
    bool isColliding(const Primitive& primitive) const;
    bool isColliding(const AxisAlignedBoundingBox& other) const;

    bool isInside(const Vec3& pos) const;
    bool isInside(const Primitive& primitive) const;
    bool isInside(const AxisAlignedBoundingBox& other) const;

    void unite(const Vec3& lowerLimit, const Vec3& upperLimit);
    void unite(const Vec3& pos);
    void unite(const Primitives& primitives);
    void unite(const AxisAlignedBoundingBox& other);

    void update(const Nodes& nodes);
    void update(const Primitives& primitives);
    void update(const AxisAlignedBoundingBox& other);

    void resize(float resizeFactor);

    void fixOutNodes(Nodes& nodes);
    void delimit(Nodes& nodes);
    void delimitIfCollide(Nodes& nodes);

protected:
    void _clear();

    bool _isColliding(const Vec3& lowerLimit, const Vec3& upperLimit) const;

    bool _isInside(const Vec3& lowerLimit, const Vec3& upperLimit) const;

protected:
    Vec3 _lowerLimit;

    Vec3 _upperLimit;
};

}  // namespace geometry
}  // namespace phyanim

#endif  // __PHYANIM_AXIS_ALIGNED_BOUNDING_BOX__
