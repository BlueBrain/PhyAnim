#ifndef __EXAMPLES_GLFW_APP__
#define __EXAMPLES_GLFW_APP__

#include <GLFW/glfw3.h>

#include "Scene.h"

namespace examples
{
class GLFWApp
{
public:
    GLFWApp(int argc, char** argv);

    virtual ~GLFWApp();

    void run();

protected:
    void _renderLoop();

    virtual void _actionLoop();

    static void _actionThread(GLFWApp* app);

    void _loadMeshes(std::vector<std::string> files);

    void _writeMeshes(phyanim::Meshes meshes,
                      std::vector<std::string> files,
                      std::string extension = "");

    void _sortAABBs(phyanim::AxisAlignedBoundingBoxes& aabbs)
    {
        auto cmp = [](phyanim::AxisAlignedBoundingBoxPtr a,
                      phyanim::AxisAlignedBoundingBoxPtr b) {
            return a->radius() > b->radius();
        };
        std::sort(aabbs.begin(), aabbs.end(), cmp);
    }

    bool _getAnim();

    void _setAnim(bool anim);

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

    virtual void _mouseScrollCallback(GLFWwindow* window,
                                      double xoffset,
                                      double yoffset);

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

    static void _wrapperMouseScrollCallback(GLFWwindow* window,
                                            double xoffset,
                                            double yoffset);

    static void glfwError(int id, const char* description)
    {
        std::cout << description << std::endl;
    }

protected:
    GLFWwindow* _window;

    Scene* _scene;

    uint32_t _width;
    uint32_t _height;

    std::vector<std::string> _args;

    double _mouseX;
    double _mouseY;

    bool _leftButtonPressed;
    bool _middleButtonPressed;
    bool _rightButtonPressed;

    bool _anim;
    std::mutex _animMutex;

    phyanim::AxisAlignedBoundingBox _limits;
    phyanim::Meshes _meshes;
    phyanim::AxisAlignedBoundingBoxes _aabbs;
    int32_t _collisionId;
    double _bbFactor;
};

}  // namespace examples

#endif
