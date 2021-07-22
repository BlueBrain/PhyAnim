#ifndef __EXAMPLES_OVERLAP_APP__
#define __EXAMPLES_OVERLAP_APP__

#include <AnimSystem.h>
#include <GLFWApp.h>

#include <chrono>

namespace examples
{
class OverlapApp : public GLFWApp
{
public:
    OverlapApp();

    void init(int argc, char** argv);

    void loop();

protected:
    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

private:
    phyanim::AnimSystem* _animSys;

    phyanim::CollisionDetection* _collisionSys;

    phyanim::Mesh* _mesh;
    std::string _file;

    phyanim::Meshes _staticMeshes;

    phyanim::Meshes _meshes;
    std::vector<std::string> _files;

    bool _anim;

    std::chrono::time_point<std::chrono::steady_clock> _startTime;

    double _stiffness;
    double _damping;
    double _density;
    double _poissonRatio;
    double _collisionStiffness;
    double _dt;
};

}  // namespace examples

#endif
