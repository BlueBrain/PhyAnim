#ifndef __EXAMPLES_DEMO_APP__
#define __EXAMPLES_DEMO_APP__

#include <GLFWApp.h>

namespace examples
{
class DemoApp : public GLFWApp
{
public:
    DemoApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _coloredMeshes();

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
    phyanim::Mesh* _mesh;
};

}  // namespace examples

#endif
