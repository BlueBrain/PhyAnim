#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <Camera.h>
#include <DrawableMesh.h>
#include <MassSpringSystem.h>

namespace examples{

class Scene{
  
  public:
  
    Scene(Camera* camera_, float dt_ = 1.0f/60, float meshStiffness_ = 1000.0f);

    virtual ~Scene(void);

    float dt(void);

    void dt(float dt_);
    
    void render(void);

    void restart(void);
    
    void gravity(void);

    void floorCollision(void);
    
  private:

    void _init(void);

    unsigned int _compileShader(const std::string& source_, int type_);

    Camera* _camera;
    phyanim::DrawableMesh* _mesh;
    phyanim::AnimSystem* _animSys;
    float _dt;
    float _meshStiffness;
    
    unsigned int _program;
    int _uProjViewModel;
    int _uViewModel;
};

}

#endif
