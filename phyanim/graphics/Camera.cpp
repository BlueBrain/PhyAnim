/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
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

#include "Camera.h"

#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace phyanim
{
namespace graphics
{
const float pi = 2.0 * acos(0.0);
const float degrees2radians = pi / 180.0;

Camera::Camera(geometry::Vec3 position,
               geometry::Mat3 rotation,
               float distance,
               float fov,
               float ratio)
    : _position(position)
    , _rotation(rotation)
    , _distance(distance)
    , _ratio(ratio)
    , _fov(fov * degrees2radians * 0.5f)
{
    _makeViewMat();
    _makeProjectionMat();
}

Camera::~Camera() {}

geometry::Vec3 Camera::position() const { return _position; }

void Camera::position(geometry::Vec3 position_)
{
    _position = position_;
    _makeViewMat();
}

geometry::Mat3 Camera::rotation() const { return _rotation; }

void Camera::rotation(geometry::Mat3 rotation)
{
    _rotation = rotation;
    _makeViewMat();
}

float Camera::distance() const { return _distance; };

void Camera::distance(float distance)
{
    _distance = distance;
    _makeViewMat();
};

void Camera::fov(float fov_)
{
    _fov = fov_ * degrees2radians * 0.5f;
    _makeProjectionMat();
}

void Camera::ratio(float ratio_)
{
    _ratio = ratio_;
    _makeProjectionMat();
}

geometry::Mat4 Camera::viewMatrix() const { return _viewMat; }

geometry::Mat4 Camera::projectionMatrix() const { return _projectionMat; }

geometry::Mat4 Camera::projectionViewMatrix() const
{
    return _projectionMat * _viewMat;
}

void Camera::_makeViewMat()
{
    geometry::Vec3 p =
        (glm::transpose(_rotation) * geometry::Vec3(0.0f, 0.0f, _distance) +
         _position);
    _viewMat = geometry::Mat4(_rotation);
    _viewMat = glm::translate(_viewMat, -p);
}

void Camera::_makeProjectionMat()
{
    float nearPlane = 0.01;
    float farPlane = 10000.0;
    _projectionMat = glm::perspective(_fov, _ratio, nearPlane, farPlane);
}

}  // namespace graphics
}  // namespace phyanim
