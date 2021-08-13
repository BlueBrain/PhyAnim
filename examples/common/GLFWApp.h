#ifndef __EXAMPLES_GLFW_APP__
#define __EXAMPLES_GLFW_APP__

#include <GLFW/glfw3.h>

#include "Scene.h"

namespace examples
{
class GLFWApp
{
public:
    GLFWApp();

    virtual ~GLFWApp();

    void init(int argc, char** argv);

    void loop();

protected:
    void _setCameraPos(phyanim::AxisAlignedBoundingBox limits);

    virtual void _keyCallback(GLFWwindow* window,
                              int key,
                              int scancode,
                              int action,
                              int mods);

    virtual void _resizeCallback(GLFWwindow* window, int width, int height);

    virtual void _mouseButtonCallback(GLFWwindow* window,
                                      int button,
                                      int action,
                                      int mods);

    virtual void _mousePositionCallback(GLFWwindow* window,
                                        double xpos,
                                        double ypos);

private:
    void _initGLFW();

    static void _wrapperKeyCallback(GLFWwindow* window,
                                    int key,
                                    int scancode,
                                    int action,
                                    int mods);

    static void _wrapperResizeCallback(GLFWwindow* window,
                                       int width,
                                       int height);

    static void _wrapperMouseButtonCallback(GLFWwindow* window,
                                            int button,
                                            int action,
                                            int mods);

    static void _wrapperMousePositionCallback(GLFWwindow* window,
                                              double xpos,
                                              double ypos);

protected:
    GLFWwindow* _window;

    Scene* _scene;

    double _mouseX;
    double _mouseY;

    bool _leftButtonPressed;
    bool _rightButtonPressed;

    double _cameraPosInc;

    phyanim::AxisAlignedBoundingBox _limits;
};

}  // namespace examples

#endif
