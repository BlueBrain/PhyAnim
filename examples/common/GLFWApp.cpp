// clang-format off
#include <GL/glew.h>
// clang-format on

#include "GLFWApp.h"

#include <iostream>
#include <thread>

namespace examples
{
GLFWApp::GLFWApp(int argc, char** argv)
    : _scene(nullptr)
    , _mouseX(0.0)
    , _mouseY(0.0)
    , _leftButtonPressed(false)
    , _middleButtonPressed(false)
    , _rightButtonPressed(false)
    , _cameraPosInc(0.1)
    , _anim(false)
{
    _initGLFW();
    _scene = new Scene();
    for (uint32_t i = 1; i < argc; ++i) _args.push_back(std::string(argv[i]));
}

GLFWApp::~GLFWApp()
{
    if (_scene) delete _scene;
}

void GLFWApp::run()
{
    std::thread actionTask(_actionThread, this);

    // _actionLoop();
    _renderLoop();
    // actionTask.detach();
}

void GLFWApp::_renderLoop()
{
    while (!glfwWindowShouldClose(_window))
    {
        glfwMakeContextCurrent(_window);
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void GLFWApp::_actionLoop()
{
    phyanim::AxisAlignedBoundingBox limits;

    for (auto file : _args)
    {
        auto mesh = new phyanim::DrawableMesh();
        std::cout << "Loading file " << file << std::endl;
        mesh->load(file);
        limits.unite(phyanim::AxisAlignedBoundingBox(mesh->surfaceTriangles));
        _scene->addMesh(mesh);
        _setCameraPos(limits);
    }
}

void GLFWApp::_actionThread(GLFWApp* app) { app->_actionLoop(); }

bool GLFWApp::_getAnim()
{
    _animMutex.lock();
    bool anim = _anim;
    _animMutex.unlock();
    return anim;
}

void GLFWApp::_setAnim(bool anim)
{
    _animMutex.lock();
    _anim = anim;
    _animMutex.unlock();
}

void GLFWApp::_setCameraPos(phyanim::AxisAlignedBoundingBox limits)
{
    phyanim::Vec3 cameraPos = limits.center();
    phyanim::Vec3 dist = limits.upperLimit() - cameraPos;
    double max = std::max(std::max(dist.x(), dist.y()), dist.z());
    cameraPos.z() = limits.upperLimit().z() + max;
    _cameraPosInc = max * 0.001;
    _scene->cameraPosition(cameraPos);
}

void GLFWApp::_initGLFW()
{
    if (!glfwInit()) throw std::runtime_error("GLFW could not be init");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    _window = glfwCreateWindow(600, 600, "Example app", NULL, NULL);

    if (!_window)
    {
        glfwTerminate();
        throw std::runtime_error("GLFW could not create a window");
    }

    glfwMakeContextCurrent(_window);
    // glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    glewInit();

    auto version = glGetString(GL_VERSION);

    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, GLFWApp::_wrapperKeyCallback);
    glfwSetWindowSizeCallback(_window, GLFWApp::_wrapperResizeCallback);
    glfwSetMouseButtonCallback(_window, GLFWApp::_wrapperMouseButtonCallback);
    glfwSetCursorPosCallback(_window, GLFWApp::_wrapperMousePositionCallback);
}

void GLFWApp::_wrapperKeyCallback(GLFWwindow* window,
                                  int key,
                                  int scancode,
                                  int action,
                                  int mods)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_keyCallback(window, key, scancode, action, mods);
}

void GLFWApp::_keyCallback(GLFWwindow* window,
                           int key,
                           int scancode,
                           int action,
                           int mods)
{
    if (_scene)
    {
        phyanim::Vec3 dxyz(0.0, 0.0, 0.0);
        bool cameraDisplaced = false;

        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case 'M':
                _scene->changeRenderMode();
                break;
            case 'F':
                _scene->showFPS = !_scene->showFPS;
                break;
            case GLFW_KEY_SPACE:
                _setAnim(!_anim);
                if (_anim)
                    std::cout << "release" << std::endl;
                else
                    std::cout << "pause" << std::endl;
                break;
            }
        }
        switch (key)
        {
        case 'W':
            dxyz += phyanim::Vec3(0.0, 0.0, -1.0);
            cameraDisplaced = true;
            break;
        case 'S':
            dxyz += phyanim::Vec3(0.0, 0.0, 1.0);
            cameraDisplaced = true;
            break;
        case 'A':
            dxyz += phyanim::Vec3(-1.0, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        case 'D':
            dxyz += phyanim::Vec3(1.0, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        }

        if (cameraDisplaced)
        {
            dxyz *= _cameraPosInc * 10.0;
            _scene->displaceCamera(dxyz);
        }
    }
}

void GLFWApp::_wrapperResizeCallback(GLFWwindow* window, int width, int height)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_resizeCallback(window, width, height);
}

void GLFWApp::_resizeCallback(GLFWwindow* window, int width, int height)
{
    if (_scene)
    {
        _scene->cameraRatio(width, height);
    }
}

void GLFWApp::_wrapperMouseButtonCallback(GLFWwindow* window,
                                          int button,
                                          int action,
                                          int mods)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mouseButtonCallback(window, button, action, mods);
}

void GLFWApp::_mouseButtonCallback(GLFWwindow* window,
                                   int button,
                                   int action,
                                   int mods)
{
    if (_scene)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                _leftButtonPressed = true;

                glfwGetCursorPos(window, &_mouseX, &_mouseY);

                uint32_t pickedId = _scene->picking(_mouseX, _mouseY);

                for (uint32_t id = 0; id < _scene->meshes.size(); ++id)
                {
                    auto mesh = _scene->meshes[id];
                    phyanim::Vec3 color(0.4, 0.4, 0.8);
                    if (id + 1 == pickedId)
                        color = phyanim::Vec3(0.8, 0.4, 0.4);
                    dynamic_cast<phyanim::DrawableMesh*>(mesh)->updateColor(
                        color);
                }
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_PRESS)
            {
                _middleButtonPressed = true;

                glfwGetCursorPos(window, &_mouseX, &_mouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                _middleButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                _rightButtonPressed = true;

                glfwGetCursorPos(window, &_mouseX, &_mouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }
        }
    }
}

void GLFWApp::_wrapperMousePositionCallback(GLFWwindow* window,
                                            double xpos,
                                            double ypos)
{
    GLFWApp* app = static_cast<GLFWApp*>(glfwGetWindowUserPointer(window));
    app->_mousePositionCallback(window, xpos, ypos);
}

void GLFWApp::_mousePositionCallback(GLFWwindow* window,
                                     double xpos,
                                     double ypos)
{
    if (_scene)
    {
        double diffX = xpos - _mouseX;
        double diffY = ypos - _mouseY;
        _mouseX = xpos;
        _mouseY = ypos;
        phyanim::Vec3 dxyz =
            phyanim::Vec3(-diffX * _cameraPosInc, diffY * _cameraPosInc, 0.0);
        if (_leftButtonPressed)
        {
        }
        if (_middleButtonPressed)
        {
            _scene->displaceCamera(dxyz);
        }
        if (_rightButtonPressed)
        {
            _scene->rotateCamera(-diffY * 0.005, -diffX * 0.005);
        }
    }
}

}  // namespace examples
