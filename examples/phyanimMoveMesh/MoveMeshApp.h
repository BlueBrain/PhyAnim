#ifndef __EXAMPLES_MOVE_MESH_APP__
#define __EXAMPLES_MOVE_MESH_APP__

#include "../common/GLFWApp.h"

namespace examples
{
class MoveMeshApp : public GLFWApp
{
public:
    MoveMeshApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _colorMeshes();

    virtual void _keyCallback(GLFWwindow* window,
                              int key,
                              int scancode,
                              int action,
                              int mods);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

    void _mousePositionCallback(GLFWwindow* window, double xpos, double ypos);

    std::vector<std::string> _fileNames;
    phyanim::Mesh* _animMesh;
    Mesh* _renderMesh;
};

}  // namespace examples

#endif
