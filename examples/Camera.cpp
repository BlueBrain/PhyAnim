#include <iostream>

#include "Camera.h"

namespace examples{

const float pi = 2.0f*acos(0.0f);
const float degrees2radians = pi/180.0f;

Camera::Camera(phyanim::Vec3 position_, float fov_, float ratio_)
        : _position(position_)
        , _ratio(ratio_)
        , _fov(fov_*degrees2radians*0.5f) {
    _makeViewMat();
    _makeProjectionMat();
}

Camera::~Camera(void){}

phyanim::Vec3 Camera::position(void) const {
    return _position;
} 

void Camera::position(phyanim::Vec3 position_) {
    _position = position_;
    _makeViewMat();
}

void Camera::fov(float fov_) {
    _fov = fov_*degrees2radians*0.5f;
    _makeProjectionMat();
}

void Camera::ratio(float ratio_) {
    _ratio = ratio_;
    _makeProjectionMat();
}

phyanim::Mat4 Camera::viewMatrix(void) const {
    return _viewMat;
}

phyanim::Mat4 Camera::projectionMatrix(void) const {
    return _projectionMat;
}

phyanim::Mat4 Camera::projectionViewMatrix(void) const {
    return _projectionMat*_viewMat;
}

void Camera::_makeViewMat(void) {

    _viewMat <<
            1.0f, .0f, .0f, -_position.x( ),
            .0f, 1.0f, .0f, -_position.y( ),
            .0f, .0f, 1.0f, -_position.z( ),
            0.0f, .0f, .0f, 1.0f;
}


void Camera::_makeProjectionMat(void) {
    float nearPlane = 0.0001f;
    float farPlane = 1000.0f;
    float nf = 1.0f/(nearPlane-farPlane);
    float f = 1.0f/tan(_fov);

    _projectionMat <<
            f/_ratio, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, (farPlane+nearPlane)*nf, 2.0f*farPlane*nearPlane*nf,
            0.0f, 0.0f, -1.0f, 0.0f;
}

}
