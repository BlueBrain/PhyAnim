#ifndef __EXAMPLES_OVERLAPSCENE__
#define __EXAMPLES_OVErLAPSCENE__

#include <chrono>

#include <Camera.h>
#include <DrawableMesh.h>
#include <AnimSystem.h>

#include "Scene.h"

namespace examples{

class OverlapScene{
  
  public:
  
    OverlapScene(Camera* camera_, SimSystem simSystem_ = EXPLICITMASSSPRING,
                 double dt_ = 0.01, double meshStiffness_ = 1000.0,
                 double meshDensity_ = 10.0, double meshDamping_ = 0.1, 
                 double meshPoissonRatio_ = 0.499, double collisionStiffness_ = 1000.0);

    virtual ~OverlapScene(void);

    double dt(void);

    void dt(float dt_);
    
    void render(void);

    void loadMesh(const std::string& file_, phyanim::Vec3 translation_ = phyanim::Vec3::Zero());

    void writeMesh(const std::string& file_, phyanim::Mesh* mesh_);
    
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


    std::chrono::time_point<std::chrono::steady_clock> _startTime;
};

}

#endif
