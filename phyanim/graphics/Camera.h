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

#ifndef __EXAMPLES_PHYANIM__
#define __EXAMPLES_PHYANIM__

#include "../geometry/Math.h"

namespace phyanim
{
namespace graphics
{
class Camera
{
public:
    Camera(phyanim::geometry::Vec3 position_ = phyanim::geometry::Vec3(),
           phyanim::geometry::Mat3 rotation = phyanim::geometry::Mat3(1.0),
           float distance = 1.0f,
           float fov_ = 90.0,
           float ratio_ = 1.0);

    ~Camera(void);

    phyanim::geometry::Vec3 position(void) const;
    void position(phyanim::geometry::Vec3 position_);

    phyanim::geometry::Mat3 rotation() const;
    void rotation(phyanim::geometry::Mat3 rotation);

    float distance() const;
    void distance(float distance);

    void fov(float fov_);

    void ratio(float ratio_);

    phyanim::geometry::Mat4 viewMatrix(void) const;

    phyanim::geometry::Mat4 projectionMatrix(void) const;

    phyanim::geometry::Mat4 projectionViewMatrix(void) const;

private:
    void _makeViewMat(void);

    void _makeProjectionMat(void);

    phyanim::geometry::Vec3 _position;
    float _distance;
    phyanim::geometry::Mat3 _rotation;
    float _fov;
    float _ratio;

    phyanim::geometry::Mat4 _viewMat;
    phyanim::geometry::Mat4 _projectionMat;
};

}  // namespace graphics
}  // namespace phyanim

#endif
