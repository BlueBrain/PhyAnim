/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __PHYANIM_MATH__
#define __PHYANIM_MATH__

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace phyanim
{
namespace geometry
{
typedef glm::vec4 Vec4;
typedef glm::vec3 Vec3;
typedef glm::vec2 Vec2;

typedef glm::mat4 Mat4;
typedef glm::mat3 Mat3;

const static float minfloat = std::numeric_limits<float>::lowest();
const static float maxfloat = std::numeric_limits<float>::max();

const static Vec3 minVec3 = Vec3(minfloat, minfloat, minfloat);
const static Vec3 maxVec3 = Vec3(maxfloat, maxfloat, maxfloat);

float clamp(const float value, const float low, const float up);

Vec3 mix(const Vec3& a, const Vec3& b, const float t);

Vec3 project(const Vec3& p, const Vec3& seg0, const Vec3& seg1, float& t);

void project(const Vec3& a,
             const Vec3& b,
             const Vec3& c,
             const Vec3& d,
             Vec3& p0,
             float& t0,
             Vec3& p1,
             float& t1);

}  // namespace geometry
}  // namespace phyanim

#endif  // __PHYANIM_MATH__