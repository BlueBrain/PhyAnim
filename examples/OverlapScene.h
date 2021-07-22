#ifndef __EXAMPLES_OVERLAPSCENE__
#define __EXAMPLES_OVERLAPSCENE__

#include <AnimSystem.h>
#include <Camera.h>
#include <DrawableMesh.h>

#include <chrono>

#include "Scene.h"

namespace examples
{
class OverlapScene
{
public:
    OverlapScene(const std::vector<std::string>& files_,
                 Camera* camera_,
                 SimSystem simSystem_ = EXPLICITMASSSPRING,
                 double dt_ = 0.01,
                 double meshStiffness_ = 1000.0,
                 double meshDensity_ = 10.0,
                 double meshDamping_ = 0.1,
                 double meshPoissonRatio_ = 0.499,
                 double collisionStiffness_ = 1000.0);

    virtual ~OverlapScene(void);

    void render(void);

    void changeRenderMode(void);

private:
    unsigned int _compileShader(const std::string& source_, int type_);

    phyanim::DrawableMesh* _loadMesh(const std::string& file_);
    phyanim::DrawableMesh* _loadMesh(const std::string& nodeFile_,
                                     const std::string& eleFile_);

public:
    bool anim;

private:
    Camera* _camera;
    std::string _file;
    std::string _file1;
    phyanim::Mesh* _mesh;
    std::vector<std::string> _files;
    phyanim::Meshes _meshes;
    phyanim::AnimSystem* _animSys;
    phyanim::CollisionDetection* _collDetect;
    double _dt;
    double _meshStiffness;
    double _meshDamping;
    double _meshPoissonRatio;
    double _meshDensity;
    double _collisionStiffness;

    bool _solved;

    unsigned int _program;
    int _uProjViewModel;
    int _uViewModel;

    unsigned int _numRenderMode;
    unsigned int _renderMode;

    std::chrono::time_point<std::chrono::steady_clock> _startTime;
};

}  // namespace examples

#endif
