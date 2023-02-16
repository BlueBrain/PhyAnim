#include "Math.h"

namespace phyanim
{

double clamp(const double value, const double low, const double up)
{
    double v = value;
    if (v < low) v = low;
    if (v > up) v = up;
    return v;
}

Vec3 mix(const Vec3& a, const Vec3& b, const double t)
{
    return a * (1.0 - t) + b * t;
}

Vec3 project(const Vec3& p, const Vec3& a, const Vec3& b, double& t)
{
    Vec3 ba = b - a;
    double l = ba.norm();
    if (l == 0.0)
    {
        t = 0.5;
        return a;
    }
    t = (p - a).dot(ba) / (l * l);
    t = clamp(t, 0.0, 1.0);
    return mix(a, b, t);
}

void project(const Vec3& a,
             const Vec3& b,
             const Vec3& c,
             const Vec3& d,
             Vec3& p0,
             double& t0,
             Vec3& p1,
             double& t1)
{
    Vec3 ba = b - a;
    double l0 = ba.norm();
    if (l0 == 0.0)
    {
        t0 = 0.5;
        p0 = a;
        p1 = project(p0, c, d, t1);
        return;
    }

    Vec3 dc = d - c;
    double l1 = dc.norm();
    if (l1 == 0.0)
    {
        t1 = 0.5;
        p1 = c;
        p0 = project(p1, a, b, t0);
        return;
    }

    Vec3 a_proj = a - (dc * ((a - c).dot(dc) / (l1 * l1)));
    Vec3 b_proj = b - (dc * ((b - c).dot(dc) / (l1 * l1)));
    Vec3 ba_proj = b_proj - a_proj;
    double lbap = ba_proj.norm();
    if (lbap == 0)
        t0 = 0.0;
    else
        t0 = (c - a_proj).dot(ba_proj) / (lbap * lbap);
    t0 = clamp(t0, 0.0, 1.0);
    p0 = mix(a, b, t0);

    p1 = project(p0, c, d, t1);
    p0 = project(p1, a, b, t0);
}

}  // namespace phyanim
