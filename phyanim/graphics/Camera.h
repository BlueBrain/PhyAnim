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
