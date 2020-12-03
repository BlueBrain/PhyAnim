#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <chrono>

#include <Camera.h>
#include <DrawableMesh.h>
#include <AnimSystem.h>

namespace examples{

typedef enum{
    EXPLICITMASSSPRING = 0,
    IMPLICITMASSSPRING,
    EXPLICITFEM,
    IMPLICITFEM
} SimSystem;

class Scene{
  
  public:
  
    Scene(Camera* camera_, SimSystem simSystem_ = EXPLICITMASSSPRING,
          double dt_ = 0.01, double meshStiffness_ = 1000.0,
          double meshDensity_ = 10.0, double meshDamping_ = 0.1, 
          double meshPoissonRatio_ = 0.499, double collisionStiffness_ = 1000.0);

    virtual ~Scene(void);

    double dt(void);

    void dt(float dt_);
    
    void render(void);

    void loadMesh(const std::string& file_);

    void clear(void);
    
    void restart(void);
    
    void gravity(void);

    void collisions(void);

    void changeRenderMode(void);

    bool anim(void);

    void anim(bool anim_);
    
  private:

    unsigned int _compileShader(const std::string& source_, int type_);

    Camera* _camera;
    std::vector<phyanim::DrawableMesh*> _meshes;
    phyanim::AnimSystem* _animSys;
    phyanim::CollisionDetection* _collDetect;
    double _dt;
    double _meshStiffness;
    double _meshDamping;
    double _meshPoissonRatio;
    double _meshDensity;
    double _collisionStiffness;
    
    unsigned int _program;
    int _uProjViewModel;
    int _uViewModel;

    unsigned int _numRenderMode;
    unsigned int _renderMode;
    bool _anim;

    double _meshVolume;

    std::chrono::time_point<std::chrono::steady_clock> _previousTime;

    bool _collision;
    unsigned int _framesCount;
    double _simulationTime;
    double _meshUpdateTime;
};

}

#endif
