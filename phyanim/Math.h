#ifndef __PHYANIM_MATH__
#define __PHYANIM_MATH__

#include <Eigen/Dense>
#include <algorithm>

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

double clamp(const double value, const double low, const double up);

Vec3 mix(const Vec3& a, const Vec3& b, const double t);

Vec3 project(const Vec3& p, const Vec3& seg0, const Vec3& seg1, double& t);

void project(const Vec3& a,
             const Vec3& b,
             const Vec3& c,
             const Vec3& d,
             Vec3& p0,
             double& t0,
             Vec3& p1,
             double& t1);

}  // namespace phyanim

#endif  // __PHYANIM_MATH__