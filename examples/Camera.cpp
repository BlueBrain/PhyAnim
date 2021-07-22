#include "Camera.h"

#include <iostream>

namespace examples
{
const double pi = 2.0 * acos(0.0);
const double degrees2radians = pi / 180.0;

Camera::Camera(phyanim::Vec3 position,
               phyanim::Mat3 rotation,
               double fov,
               double ratio)
    : _position(position)
    , _rotation(rotation)
    , _ratio(ratio)
    , _fov(fov * degrees2radians * 0.5f)
{
    _makeViewMat();
    _makeProjectionMat();
}

Camera::~Camera() {}

phyanim::Vec3 Camera::position() const { return _position; }

void Camera::position(phyanim::Vec3 position_)
{
    _position = position_;
    _makeViewMat();
}

phyanim::Mat3 Camera::rotation() const { return _rotation; }

void Camera::rotation(phyanim::Mat3 rotation)
{
    _rotation = rotation;
    _makeViewMat();
}

void Camera::fov(double fov_)
{
    _fov = fov_ * degrees2radians * 0.5f;
    _makeProjectionMat();
}

void Camera::ratio(double ratio_)
{
    _ratio = ratio_;
    _makeProjectionMat();
}

phyanim::Mat4 Camera::viewMatrix() const { return _viewMat; }

phyanim::Mat4 Camera::projectionMatrix() const { return _projectionMat; }

phyanim::Mat4 Camera::projectionViewMatrix() const
{
    return _projectionMat * _viewMat;
}

void Camera::_makeViewMat()
{
    phyanim::Mat3 inv = _rotation.inverse();
    _viewMat = phyanim::Mat4::Identity();
    _viewMat.block<3, 3>(0, 0) = inv;
    _viewMat.block<3, 1>(0, 3) = inv * (-_position);
}

void Camera::_makeProjectionMat()
{
    double nearPlane = 0.0001;
    double farPlane = 1000.0;
    double nf = 1.0 / (nearPlane - farPlane);
    double f = 1.0 / tan(_fov);

    _projectionMat << f / _ratio, 0.0, 0.0, 0.0, 0.0, f, 0.0, 0.0, 0.0, 0.0,
        (farPlane + nearPlane) * nf, 2.0 * farPlane * nearPlane * nf, 0.0, 0.0,
        -1.0, 0.0;
}

}  // namespace examples
