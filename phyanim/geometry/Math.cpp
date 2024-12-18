/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Math.h"

namespace phyanim
{
namespace geometry
{
#define THRESHOLD 0.001f

float clamp(const float value, const float low, const float up)
{
    float v = value;
    if (v < low) v = low;
    if (v > up) v = up;
    return v;
}

Vec3 mix(const Vec3& a, const Vec3& b, const float t)
{
    return a * (1.0f - t) + b * t;
}

Vec3 project(const Vec3& p, const Vec3& a, const Vec3& b, float& t)
{
    float l = glm::distance(b, a);
    if (l < THRESHOLD)
    {
        t = 0.0f;
        return a;
    }
    Vec3 ba = b - a;
    t = glm::dot(p - a, ba) / (l * l);
    t = clamp(t, 0.0, 1.0);
    return mix(a, b, t);
}

void project(const Vec3& a,
             const Vec3& b,
             const Vec3& c,
             const Vec3& d,
             Vec3& p0,
             float& t0,
             Vec3& p1,
             float& t1)
{
    float l0 = glm::distance(b, a);
    if (l0 < THRESHOLD)
    {
        t0 = 0.0f;
        p0 = a;
        p1 = project(p0, c, d, t1);
        return;
    }

    Vec3 ba = b - a;
    float l1 = glm::distance(d, c);
    if (l1 < THRESHOLD)
    {
        t1 = 0.0f;
        p1 = c;
        p0 = project(p1, a, b, t0);
        return;
    }
    Vec3 dc = d - c;
    Vec3 a_proj = a - (dc * (glm::dot(a - c, dc) / (l1 * l1)));
    Vec3 b_proj = b - (dc * (glm::dot(b - c, dc) / (l1 * l1)));
    Vec3 ba_proj = b_proj - a_proj;
    float lbap = glm::distance(b_proj, a_proj);
    if (lbap < THRESHOLD)
        t0 = 0.0f;
    else
        t0 = glm::dot(c - a_proj, ba_proj) / (lbap * lbap);
    t0 = clamp(t0, 0.0f, 1.0f);
    p0 = mix(a, b, t0);

    p1 = project(p0, c, d, t1);
    p0 = project(p1, a, b, t0);
}

}  // namespace geometry
}  // namespace phyanim
