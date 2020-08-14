#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <Camera.h>
#include <DrawableMesh.h>
#include <MassSpringSystem.h>
#include <FEMSystem.h>

namespace examples{

typedef enum{
    MASSSPRING = 0,
    FEM
} SimSystem;

class Scene{
  
  public:
  
    Scene(Camera* camera_, SimSystem simSystem_ = MASSSPRING,
          double dt_ = 0.01, double meshStiffness_ = 1000.0,
          double meshDensity_ = 10.0, double meshDamping_ = 0.1, 
          double meshPoissonRatio_ = 0.499);

    virtual ~Scene(void);

    double dt(void);

    void dt(float dt_);
    
    void render(void);

    void loadMesh(const std::string& file_);
    
    void restart(void);
    
    void gravity(void);

    void floorCollision(void);

    void changeRenderMode(void);

    bool anim(void);

    void anim(bool anim_);
    
  private:

    unsigned int _compileShader(const std::string& source_, int type_);

    Camera* _camera;
    phyanim::DrawableMesh* _mesh;
    phyanim::AnimSystem* _animSys;
    double _dt;
    double _meshStiffness;
    double _meshDamping;
    double _meshPoissonRatio;
    double _meshDensity;
    
    unsigned int _program;
    int _uProjViewModel;
    int _uViewModel;

    unsigned int _numRenderMode;
    unsigned int _renderMode;
    bool _anim;
};

}

#endif
