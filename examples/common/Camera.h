#ifndef __EXAMPLES_CAMERA__
#define __EXAMPLES_CAMERA__

#include <Phyanim.h>

namespace examples
{
class Camera
{
public:
    Camera(phyanim::Vec3 position_ = phyanim::Vec3::Zero(),
           phyanim::Mat3 rotation = phyanim::Mat3::Identity(),
           float distance = 1.0f,
           double fov_ = 90.0,
           double ratio_ = 1.0);

    ~Camera(void);

    phyanim::Vec3 position(void) const;
    void position(phyanim::Vec3 position_);

    phyanim::Mat3 rotation() const;
    void rotation(phyanim::Mat3 rotation);

    float distance() const;
    void distance(float distance);

    void fov(double fov_);

    void ratio(double ratio_);

    phyanim::Mat4 viewMatrix(void) const;

    phyanim::Mat4 projectionMatrix(void) const;

    phyanim::Mat4 projectionViewMatrix(void) const;

private:
    void _makeViewMat(void);

    void _makeProjectionMat(void);

    phyanim::Vec3 _position;
    float _distance;
    phyanim::Mat3 _rotation;
    double _fov;
    double _ratio;

    phyanim::Mat4 _viewMat;
    phyanim::Mat4 _projectionMat;
};

}  // namespace examples

#endif
