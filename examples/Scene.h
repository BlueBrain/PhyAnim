#ifndef __EXAMPLES_SCENE__
#define __EXAMPLES_SCENE__

#include <chrono>

#include <Camera.h>
#include <DrawableMesh.h>
#include <RenderProgram.h>

namespace examples{

typedef enum RenderMode
{
    SOLID = 0,
    WIREFRAME
} RenderMode;

class Scene
{
  public:

    Scene(Camera* camera);

    virtual ~Scene(void);

    void render(void);

    void addMesh(phyanim::DrawableMesh* mesh);

    void clear(void);

    RenderMode renderMode(void);

    void renderMode(RenderMode renderMode);

  private:

    Camera* _camera;

    std::vector<phyanim::DrawableMesh*> _meshes;

    RenderMode _renderMode;

    RenderProgram* _program;

    uint64_t _framesCount;

    std::chrono::time_point<std::chrono::steady_clock> _previousTime; 
};



}

#endif 
// typedef enum{
//     EXPLICITMASSSPRING = 0,
//     IMPLICITMASSSPRING,
//     EXPLICITFEM,
//     IMPLICITFEM
// } SimSystem;

// class Scene{
  
//   public:
  
//     Scene(Camera* camera_, SimSystem simSystem_ = EXPLICITMASSSPRING,
//           double dt_ = 0.01, double meshStiffness_ = 1000.0,
//           double meshDensity_ = 10.0, double meshDamping_ = 0.1, 
//           double meshPoissonRatio_ = 0.499, double collisionStiffness_ = 1000.0);

//     virtual ~Scene(void);

//     double dt(void);

//     void dt(float dt_);
    
//     void render(void);

//     void loadMesh(const std::string& file_);
    
//     void loadMesh(const std::string& nodeFile_, const std::string& eleFile_);

//     void clear(void);
    
//     void restart(void);
    
//     void gravity(void);

//     void collisions(void);

//     void changeRenderMode(void);

//     bool anim(void);

//     void anim(bool anim_);
    
//   private:

//     unsigned int _compileShader(const std::string& source_, int type_);

//     void _loadMesh(phyanim::DrawableMesh* mesh_);

//     Camera* _camera;
//     std::vector<phyanim::Mesh*> _meshes;
//     phyanim::AnimSystem* _animSys;
//     phyanim::CollisionDetection* _collDetect;
//     double _dt;
//     double _meshStiffness;
//     double _meshDamping;
//     double _meshPoissonRatio;
//     double _meshDensity;
//     double _collisionStiffness;

//     double* _tetrahedraVolume;
    
//     unsigned int _program;
//     int _uProjViewModel;
//     int _uViewModel;

//     unsigned int _numRenderMode;
//     unsigned int _renderMode;
//     bool _anim;

//     double _meshVolume;

//     std::chrono::time_point<std::chrono::steady_clock> _previousTime; 

//     bool _collision;
//     unsigned int _framesCount;
//     double _simulationTime;
//     double _meshUpdateTime;
// };

// }

// #endif
