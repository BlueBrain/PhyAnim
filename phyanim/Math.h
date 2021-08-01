#ifndef __PHYANIM_MATH__
#define __PHYANIM_MATH__

#include <Eigen/Dense>

namespace phyanim
{
typedef Eigen::Vector4d Vec4;
typedef Eigen::Vector3d Vec3;
typedef Eigen::Vector2d Vec2;

typedef Eigen::Matrix4d Mat4;
typedef Eigen::Matrix3d Mat3;

const static double minDouble = std::numeric_limits<double>::lowest();
const static double maxDouble = std::numeric_limits<double>::max();

const static Vec3 minVec3 = Vec3(minDouble, minDouble, minDouble);
const static Vec3 maxVec3 = Vec3(maxDouble, maxDouble, maxDouble);

}  // namespace phyanim

#endif  // __PHYANIM_MATH__