#ifndef __EXAMPLES_MOVE_MESH_APP__
#define __EXAMPLES_MOVE_MESH_APP__

#include "../common/GLFWApp.h"

using namespace phyanim;

namespace examples
{
class SceneGeneratorApp : public GLFWApp
{
public:
    SceneGeneratorApp(int argc, char** argv);

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

    void _mousePositionCallback(GLFWwindow* window, float xpos, float ypos);

    std::vector<std::string> _fileNames;
    geometry::Mesh* _animMesh;
    graphics::Mesh* _renderMesh;

    std::chrono::steady_clock::time_point _pickingTime;
};

}  // namespace examples

#endif
