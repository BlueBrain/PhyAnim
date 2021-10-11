#ifndef __EXAMPLES_MOVE_MESH_APP__
#define __EXAMPLES_MOVE_MESH_APP__

#include <GLFWApp.h>

namespace examples
{
class MoveMeshApp : public GLFWApp
{
public:
    MoveMeshApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _coloredMeshes();

    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

    void _mousePositionCallback(GLFWwindow* window, double xpos, double ypos);

    phyanim::Meshes _meshes;
    std::vector<std::string> _fileNames;
    phyanim::Mesh* _mesh;

    phyanim::AxisAlignedBoundingBoxes _aabbs;
    uint32_t _collisionId;
    uint32_t _bbFactor;
    phyanim::AxisAlignedBoundingBox _limits;
    std::chrono::steady_clock::time_point _pickingTime;
};

}  // namespace examples

#endif
