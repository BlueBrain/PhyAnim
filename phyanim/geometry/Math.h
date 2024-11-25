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