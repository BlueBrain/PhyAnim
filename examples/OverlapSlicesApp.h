#ifndef __EXAMPLES_OVERLAP_SLICES_APP__
#define __EXAMPLES_OVERLAP_SLICES_APP__

#include <AnimSystem.h>
#include <GLFWApp.h>

#include <chrono>

namespace examples
{
class OverlapSlicesApp : public GLFWApp
{
public:
    OverlapSlicesApp();

    void init(int argc, char** argv);

    void loop();

protected:
    phyanim::MeshPtr _sliceMesh(phyanim::MeshPtr mesh,
                                const phyanim::AxisAlignedBoundingBox& aabb);

    void _setSurfaceNodes(phyanim::MeshPtr mesh);

    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

private:
    phyanim::AnimSystem* _animSys;

    phyanim::Meshes _meshes;
    phyanim::Meshes _collisionMeshes;

    std::vector<std::string> _inFiles;
    std::vector<std::string> _outFiles;

    phyanim::AxisAlignedBoundingBoxes _aabbs;

    bool _anim;
    bool _stepByStep;

    std::chrono::time_point<std::chrono::steady_clock> _startTime;

    double _stiffness;
    double _damping;
    double _density;
    double _poissonRatio;
    double _collisionStiffness;
    double _dt;
    uint64_t _cellSize;
};

}  // namespace examples

#endif
