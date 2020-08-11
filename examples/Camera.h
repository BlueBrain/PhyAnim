#ifndef __EXAMPLES_CAMERA__
#define __EXAMPLES_CAMERA__

#include <Node.h>

namespace examples{

class Camera{

  public:

    Camera(phyanim::Vec3 position_ = phyanim::Vec3(),
           float fov_ = 90.0f,
           float ratio_ = 1.0f);

    ~Camera(void);

    phyanim::Vec3 position(void) const;
    void position(phyanim::Vec3 position_);

    void fov( float fov_ );

    void ratio( float ratio_ );

    phyanim::Mat4 viewMatrix(void) const;

    phyanim::Mat4 projectionMatrix(void) const;

    phyanim::Mat4 projectionViewMatrix(void) const;
    
  private:

    void _makeViewMat(void);

    void _makeProjectionMat(void);

    phyanim::Vec3 _position;
    float _fov;
    float _ratio;

    phyanim::Mat4 _viewMat;
    phyanim::Mat4 _projectionMat;
    
    
};

}

#endif
