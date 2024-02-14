#ifndef __EXAMPLES_DEMO_APP__
#define __EXAMPLES_DEMO_APP__

#include <phyanim/Phyanim.h>

#include "../common/GLFWApp.h"

namespace examples
{
class DemoApp : public GLFWApp
{
public:
    DemoApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _checkCollisions();

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

    void _mousePositionCallback(GLFWwindow* window, float xpos, float ypos);

    std::vector<std::string> _fileNames;
    phyanim::geometry::Mesh* _animMesh;
    phyanim::graphics::Mesh* _renderMesh;

    phyanim::graphics::ColorPalette _palette;
};

}  // namespace examples

#endif
