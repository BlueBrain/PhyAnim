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
